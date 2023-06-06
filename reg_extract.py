import re
import io
import csv

# hex-number pattern
hexp  = re.compile("^[ ]*0x[0-9a-fA-F]+$")
# bit-position range pattern
rngp  = re.compile("([0-9]+)([:]([0-9]+))?")
# optional hyphen plus new-line pattern
nlp   = re.compile('[-]?[\n]')
# Some registers (in CSV) have a index appended
multp = re.compile('^((OPN|DESIGN)_ID)[0-9]')

class DisjointPieceError(Exception):
  def __init__(self,*args, **kwargs):
    super().__init__(*args, **kwargs)

class Piece:
  def __init__(self, addr, left, right = None, **kwargs):
    super().__init__(**kwargs)
    self._addr  = [ addr ]
    self._right  = right
    if ( right is None ):
      right = left
    if ( right > left ):
      raise RuntimeError("Piece: right must be <= left")
    if not self.inSameByte(right, left):
      raise RuntimeError("Piece: right..left must be in a single byte")
    self._left = left

  @staticmethod
  def inSameByte(a,b):
    return (a >> 3) == (b >> 3)

  @property
  def right(self):
    return self._right

  @property
  def left(self):
    return self._left

  @property
  def width(self):
    return self.left - self.right + 1

  @property
  def addr(self):
    return self._addr

  def merge(self, other):
    if   ( other.right > self.left ):
      lo          = self
      hi          = other
      mergedAfter = True
    elif ( self.right  > other.left ): 
      lo          = other
      hi          = self
      mergedAfter = False
    else:
      # other.right <= self.left and other.left >= self.right
      raise RuntimeError("Merging pieces: overlap")
    if ( hi.right != lo.left + 1 ):
      raise DisjointPieceError("Cannot merge disjoint pieces")
    if ( self.inSameByte(hi.right, lo.left) ):
      if ( lo.addr[-1] != hi.addr[0] ):
        raise RuntimeError("Merging pieces found address mismatch")
      o = hi.addr[1:]
    else:
      o = hi.addr
    for a in o:
      if ( a in lo.addr ):
        raise RuntimeError("Merging pieces found overlapping register address!")
    self._addr  = lo.addr + o
    if ( mergedAfter ):
      self._left = other.left
    else:
      self._right  = other.right
    return mergedAfter

class Setting:
  def __init__(self, name, access, **kwargs):
    super().__init__(**kwargs)
    self._name  = name
    self._acc   = access
    self._pcs   = []

  @property
  def name(self):
    return self._name

  @property
  def pieces(self):
    return self._pcs

  @property
  def access(self):
    return self._acc

  def addPiece(self, o):
    if len(self._pcs) == 0:
      self._pcs.append(o)
      return self
    for i in range(len(self._pcs)):
      p = self._pcs[i]
      try:
        after = p.merge( o )
        try:
          if ( after ):
            # try to merge following piece
            i += 1
          else:
            # try to merge preceding piece
            i -= 1
          if ( i < len(self._pcs) and i >= 0 ):
            p.merge( self._pcs[i] )
            print("Removing {:d}".format(i))
            self._pcs.pop(i)
        except DisjointPieceError:
          # didnt work; we are done
          pass
        finally:
          return self
      except DisjointPieceError:
        if   ( o.left < p.right ):
          self._pcs.insert(i, o)
          return self
        elif ( o.right  > p.left ):
          self._pcs.insert(i+1, o)
          return self
        # try next
        pass
    return self

class RegSet:
  def __init__(self, fnam='regs_raw.csv', *args, **kwargs):
    super().__init__(*args, **kwargs)
    self._d = dict()
    with io.open('regs_raw.csv','r') as feil:
      for flds in csv.reader( feil ):
        if hexp.match(flds[0]):
          if (len(flds) > 2) and (flds[2] in [ "R", "R/W", "R/O", "S", "" ]):
            addr  = int(flds[0], 0)
            g     = rngp.match(flds[1]).groups()
            # our nomenclature is
            left = int(g[0])
            if not g[2] is None:
              right = int(g[2])
            else:
              right = left
            if ( len(flds[2]) != 0 ):
              acc  = flds[2]
            if acc == "R":
              acc  = "R/O"
            # else (if flds[2] empty) use previous value (table value from above)
            if ( len(flds[3]) != 0 ):
              name = flds[3]
            # else (if flds[3] empty) use previous name (table value from above)
            # Strip embedded \n
            name = nlp.sub( '', name )
            # collapse register names with an index appended
            name = multp.sub( '\\1', name )
            
            print("{}".format(name))
            if ( name == "RESERVED" ):
              continue
            el = self._d.get(name)
            if el is None:
              el = Setting(name, acc)
              self._d[name] = el
            if ( el.access != acc ):
              raise RuntimeError("Setting '{}' with mismatching access (was {}, now {})".format(name, el.access, acc))
            el.addPiece(Piece(addr, left, right))
          else:
            print("SKIPPING {}".format( ",".join(flds) ))
    # build reverse index
    self._rd = dict()
    for k,v in self._d.items():
      for p in v.pieces:
        for a in p.addr:
          settings = self._rd.get(a)
          if settings is None:
            settings = []
            self._rd[a] = settings
          if v in settings:
            raise RuntimeError("Index consistency check: same setting found multiple times for single address")
          settings.append(v)

  def consistencyCheck(self):
    flag = 0
    for k,v in self._d.items():
      # empty name?
      if len(k) == 0:
        print("Consistency check: empty name @0x{:x}".format(v.pieces[0].addr[0]))
        flag += 1
      # validate 'access'
      if not v.access in ["S", "R/O", "R/W"]:
        print("Consistency check: unexpected access: {} of {}".format( v.access, v.name ) )
      # find settings with multiple unmerged pieces
      n = len(v.pieces)
      if n > 1:
        print("Consistency check: {} has {:d} pieces".format(k, n))
        flag += 1
      for p in v.pieces:
        # make sure the length makes sense
        w = len(p.addr)*8
        if ( ( p.width > w ) or ( p.width <= w - 8 ) ):
           print("Consistency check: {} has pieces with inconsistent length {:d}".format(k, p.width))
           flag += 1
        # make sure 'right' index makes sense
        if p.right < 0 or p.right > 7:
           print("Consistency check: {} @0x{:x} has pieces with inconsistent right {:d}".format(k, p.addr[0], p.right))
           flag += 1
        # find settings that span multiple registers and share registers with other settings
        if ( w > 8 ):
          for a in p.addr:
            for s in self._rd[a]:
              if ( s != v ):
                print("Register @0x{:x} shared between {} and {}".format( a, s.name, v.name ))
                flag += 1
    return flag
    
rs=RegSet()
rs.consistencyCheck()

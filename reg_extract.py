import re
import io
import csv
import sys

# hex-number pattern
hexp  = re.compile("^[ ]*0x[0-9a-fA-F]+$")
# hex-number range pattern
hrngp = re.compile("(0x[0-9a-fA-F]+)([-](0x[0-9a-fA-F]+))?")
# bit-position range pattern
rngp  = re.compile("([0-9]+)([:]([0-9]+))?")
# optional hyphen plus new-line pattern
nlp   = re.compile('[-]?[\n]')
# Some registers (in CSV) have a index appended
multp = re.compile('^((OPN|DESIGN)_ID)[0-9]')
# prefix + suffix pattern
prsup = re.compile('^([^_]+)_(([^_]*_)*)([^_]+)$')

# comment
commp = re.compile('^[ ]*[#]')

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
  def piece(self):
    if ( len(self._pcs) != 1 ):
      raise ValueError("setting has more than one piece!")
    return self._pcs[0]

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

  def print(self, of=sys.stdout):
    p       = self.piece
    print("{:35s}: {:s} [{:2d}:{:2d}]@0x{:04x}".format(self.name, self.access, p.left, p.right, p.addr[0]), file=of)

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

    # temporary rev-index
    rd = self.buildRevIdx()

    # build 'same as' registers
    with io.open('regs_raw.csv','r') as feil:
      for flds in csv.reader( feil ):
        # single register mapping (not range)
        if len(flds) == 3 and hexp.match(flds[0]) and hexp.match(flds[2]):
          new_name_piece = flds[1].split()[0]          
          new_addr       = int(flds[0],0)
          old_addr       = int(flds[2],0)

          for s in rd[old_addr]:
            new_name = self.mapName(new_name_piece, s.name)
            p = s.piece
            if ( len(p.addr) > 1 ):
              raise RuntimeError("multiple addresses unexpected")
            if not self._d.get(new_name) is None:
              raise RuntimeError("new setting already exists")
            ns = Setting( new_name, s.access )
            ns.addPiece( Piece( new_addr, p.left, p.right ) )
            self._d[new_name] = ns
            #print("From: {:x} => {:x} ({} => {})".format(old_addr, new_addr, s.name, self.mapName(new_name_piece, s.name)))
        elif len(flds) == 4:
          mnew = hrngp.match(flds[0])
          mold = hrngp.match(flds[3])
          if ( not mnew is None and not mold is None ):
            new_name_piece = flds[1].split()[0]          
            oldf = int(mold.groups()[0],0)
            newf = int(mnew.groups()[0],0)
            go   = mold.groups()[2]
            gn   = mnew.groups()[2]
            if (go is None) != (gn is None):
              raise RuntimeError("range mapping: unrecognized old->new address mapping")
            if ( go is None ):
              oldt = oldf
              newt = newf
            else:
              oldt = int(go, 0)
              newt = int(gn, 0)
            if ( newt-newf != oldt-oldf ):
              raise RuntimeError("new settings: size of new range differs from old")
            s    = rd[oldf]
            if ( len(s) > 1 ):
              raise RuntimeError("old address mapped to multiple settings (not supported ATM)")
            else:
              s = s[0]
            ns = Setting( self.mapName( new_name_piece, s.name ), s.access )
            #print("From: {:x}-{:x} => {:x} ({} => {})".format(oldf, oldt, newf, s.name, self.mapName(new_name_piece, s.name) ))
            # assume addresses are increasing
            p = s.piece
            l = len(p.addr)
            if ( l != oldt - oldf + 1 ):
              raise RuntimeError("unexpected old address range")
            for i in range(l):
              if ( p.addr[i] != oldf + i ):
                raise RuntimeError("unexpected old address range ordering")
            r = p.right
            a = newf
            while ( r <= p.left ):
              l = r | 7
              if ( l > p.left ):
                l = p.left
              ns.addPiece( Piece( a, l, r ) )
              r  = l + 1
              a += 1
            if ( self._d.get( ns.name ) ):
              raise KeyError("newly mapped name already exists")
            self._d[ns.name] = ns

    # build complete reverse index
    self._rd = self.buildRevIdx()

  def find(self, key):
    if isinstance(key, int):
      return self._rd[key]
    else:
      return self._d[key]

  @staticmethod
  def mapName(new_name_piece, old_name):
    # heuristics: if new_name_piece already contains a '_' we assume it is the complete name
    if ( prsup.match( new_name_piece ) ):
      return new_name_piece
    if ( prsup.match( old_name ) ):
      return prsup.sub(new_name_piece + "_" + '\\2\\4', old_name)
    # old name has no '_'; new piece is the replacement
    return new_name_piece

  def buildRevIdx(self):
    # build reverse index
    rd = dict()
    for k,v in self._d.items():
      for p in v.pieces:
        for a in p.addr:
          settings = rd.get(a)
          if settings is None:
            settings = []
            rd[a] = settings
          if v in settings:
            raise RuntimeError("Index consistency check: same setting found multiple times for single address")
          settings.append(v)
    return rd

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

  # annotate a CBP register file
  def annotateCBP(self, fnam, of=sys.stdout):
    pat = re.compile("^[ ]*(0x[0-9a-fA-F]+)[ ]*[,][ ]*(0x[0-9a-fA-F]*)")
    with io.open(fnam, 'r') as f: 
      for l in f:
        m = pat.match(l)
        print(l[:-1], end = '', file=of)
        if (not m is None):
          print('# ', end='', file=of)
          a = int(m.groups()[0],0)
          v = int(m.groups()[1],0)
          try:
            for s in self.find(a):
              print(" {}".format( s.name ), end='', file=of)
          except KeyError:
            print(" *UNDOCUMENTED*", end='', file=of)
        print(file=of)

  def printSetting(self, nam, of=sys.stdout):
    setting = self._d[nam]
    setting.print( of )

  def writeCpp(self, device="Si5395", of=sys.stdout):
    print("/* THIS FILE WAS AUTOMATICALLY GENERATED (reg_extract.py) -- DO NOT MODIFY */", file = of)
    print("#include <Si53xx.h>", file = of)
    print("namespace Si53xx {", file = of)
    print("SettingVec {}Settings = {{".format(device), file = of, end='')
    fmt = '{}\n\tSetting::mkSetting("{}","{}",{:d},{:d},{{{}}})'
    beg = ''
    for k,s in self._d.items():
      p    = s.piece
      regs = ""
      for a in p.addr:
        regs += "0x{:04x},".format( a )
      print(fmt.format(beg, k, s.access, p.left, p.right, regs[:-1]), file = of, end = '')
      beg  = ','
    print("\n};", file = of)
    print("}",    file = of)

rs=RegSet()
rs.consistencyCheck()

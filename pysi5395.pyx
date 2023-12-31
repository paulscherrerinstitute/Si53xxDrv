# cython: c_string_type=str, c_string_encoding=ascii
# ^^^^^^ DO NOT REMOVE THE ABOVE LINE ^^^^^^^

from libcpp.string cimport *
from libcpp        cimport bool
from libc.stdint   cimport *
from libc.stdio    cimport *
from enum          import Enum

cdef extern from "Si5395.h" namespace "Si53xx":

  cdef cppclass MOutputConfig "Si53xx::OutputConfig":
    pass

  cdef const MOutputConfig off    "Si53xx::OutputConfig::OFF"
  cdef const MOutputConfig lvds18 "Si53xx::OutputConfig::LVDS18"
  cdef const MOutputConfig lvds25 "Si53xx::OutputConfig::LVDS25"
  cdef const MOutputConfig lvds33 "Si53xx::OutputConfig::LVDS33"

  cdef cppclass Si53xx:
    ctypedef uint64_t ValType
    ValType  get(const string &) except +
    void     set(const string &, ValType v) except+
    void     init(bool force, const string &id) except+

  cdef extern from "Si5395.h":
    cdef cppclass Si5395(Si53xx):

      Si5395() except+
      Si5395(const char *, unsigned) except+
      void     readCSV(const char *, bool) except+
      void     dumpCSV(const char *) except+
      void     dumpSettings(const char *) except+
      void     getNDivider(unsigned idx, ValType *num, ValType *den) except+
      void     setNDivider(unsigned idx, ValType num, ValType den) except+

      void     getPDivider(unsigned idx, ValType *num, ValType *den) except+
      void     setPDivider(unsigned idx, ValType num, ValType den) except+
      unsigned getRDivider(unsigned idx, bool) except+
      void     setRDivider(unsigned idx, ValType num, bool) except+

      void     getMDivider( ValType *num, ValType *den) except+
      void     setMDivider(ValType num, ValType den) except+

      void     getMXAXBDivider( ValType *num, ValType *den) except+
      void     setMXAXBDivider(ValType num, ValType den) except+

      void     sendPreamble() except+
      void     sendPostamble() except+

      void     setZDM(uint64_t, unsigned, unsigned, MOutputConfig) except+
      void     setZDM(bool) except+
      bool     getZDM() except+
      bool     isPLLOff() except+

      void     selInput(unsigned) except+

      void     setOutputMux(unsigned, unsigned) except+
      unsigned getOutputMux(unsigned, bool) except+
      void     setOutput(unsigned, MOutputConfig, unsigned, bool) except+
      bool     getOutallDisable() except+
      void     setOutallDisable(bool) except+
      bool     getOutputEnable(unsigned, bool) except+
      void     setOutputEnable(unsigned, bool, bool) except+

      unsigned getStatusLOS() except+
      unsigned getStatusOOF() except+
      bool     getStatusLOS(unsigned) except+
      bool     getStatusOOF(unsigned) except+
      bool     getStatusLOL() except+
      bool     getStatusHOLD() except+
      void     setIOVDD3V3(bool) except+
      bool     getIOVDD3V3() except+
      void     reset(bool) except+
      void     flushCache() except+
      void     syncRDividers() except+
      string   getDesignId() except+

cpdef enum OutputConfig:
  OFF    = <int>off
  LVDS18 = <int>lvds18
  LVDS25 = <int>lvds25
  LVDS33 = <int>lvds33


cdef class SI5395:
    cdef Si5395 c_cls
    cdef list   nest

    def __init__(self, busn=None, i2cAddr=0x68, str designId=""):
      if ( not busn is None ):
        self.c_cls = Si5395( busn, i2cAddr )
      else:
        self.c_cls = Si5395()
      self.c_cls.init(False, designId)
      self.nest = []

    def get(self, key):
      v = self.c_cls.get( key )
      return  v

    def set(self, key, val):
      self.c_cls.set( key, val )

    def readCSV(self, fnam, noAutoPreamble=False):
      self.c_cls.readCSV( fnam, noAutoPreamble )

    def dumpCSV(self, fnam):
      self.c_cls.dumpCSV( fnam )

    def dumpSettings(self, fnam):
      self.c_cls.dumpSettings( fnam )

    def getNDivider(self, idx):
      cdef uint64_t n,d
      self.c_cls.getNDivider(idx, &n, &d)
      return n,d

    def setNDivider(self, idx, num, den):
      self.c_cls.setNDivider(idx, num, den)

    def getPDivider(self, idx):
      cdef uint64_t n,d
      self.c_cls.getPDivider(idx, &n, &d)
      return n,d

    def setPDivider(self, idx, num, den):
      self.c_cls.setPDivider(idx, num, den)

    def getMXAXBDivider(self):
      cdef uint64_t n,d
      self.c_cls.getMXAXBDivider(&n, &d)
      return n,d

    def setMXAXBDivider(self, num, den):
      self.c_cls.setMXAXBDivider(num, den)

    def getMDivider(self):
      cdef uint64_t n,d
      self.c_cls.getMDivider(&n, &d)
      return n,d

    def getRDivider(self, idx, alt = False):
      return self.c_cls.getRDivider(idx, alt)

    def setRDivider(self, idx, val, alt=False):
      self.c_cls.setRDivider(idx, val, alt)

    def setMDivider(self, num, den):
      self.c_cls.setMDivider(num, den)

    def sendPreamble(self):
      self.c_cls.sendPreamble()

    def sendPostamble(self):
      self.c_cls.sendPostamble()

    def setOutputMux(self, idx, nDivider):
      self.c_cls.setOutputMux( idx, nDivider )

    def getOutputMux(self, idx, alt = False):
      return self.c_cls.getOutputMux( idx, alt )

    def setOutput(self, idx, OutputConfig cfg, rdiv, alt = False):
      self.c_cls.setOutput( idx, <MOutputConfig>cfg, rdiv, alt )

    def getOutallDisable(self):
      return self.c_cls.getOutallDisable()

    def setOutallDisable(self, val):
      return self.c_cls.setOutallDisable( val )

    def getOutputEnable(self, idx, alt = False):
      return self.c_cls.getOutputEnable( idx, alt )

    def setOutputEnable(self, idx, val, alt = False):
      self.c_cls.setOutputEnable( idx, val, alt )

    def selInput(self, inp):
      self.c_cls.selInput( inp )

    def setZDM(self, hzOrBool, inputSel = None, rDivider = 2, OutputConfig outputCfg=OutputConfig.LVDS33):
      if ( inputSel is None ):
        self.c_cls.setZDM( hzOrBool )
      else:
        self.c_cls.setZDM( hzOrBool, inputSel, rDivider, <MOutputConfig>outputCfg )

    def getZDM(self):
      return self.c_cls.getZDM()

    def getStatusLOS(self, idx=None):
      if (idx is None):
        return self.c_cls.getStatusLOS()
      else:
        return self.c_cls.getStatusLOS(idx)

    def getStatusOOF(self, idx=None):
      if (idx is None):
        return self.c_cls.getStatusOOF()
      else:
        return self.c_cls.getStatusOOF(idx)

    def getStatusLOL(self):
      return self.c_cls.getStatusLOL()

    def getStatusHOLD(self):
      return self.c_cls.getStatusHOLD()

    def setIOVDD3V3(self, v):
      self.c_cls.setIOVDD3V3(v)

    def getIOVDD3V3(self):
      return self.c_cls.getIOVDD3V3()

    def isPLLOff(self):
      return self.c_cls.isPLLOff()

    def __enter__(self):
      wasOff = self.isPLLOff()
      if ( not wasOff ):
        self.sendPreamble()
      self.nest.append( wasOff )
      return self

    def __exit__(self, *args):
      wasOff = self.nest.pop()
      if ( not wasOff ):
        self.sendPostamble()
      return False

    def reset(self, hard = True):
      self.c_cls.reset( hard )

    def flushCache(self):
      self.c_cls.flushCache()

    def syncRDividers(self):
      self.c_cls.syncRDividers()

    def getDesignId(self):
      return self.c_cls.getDesignId()

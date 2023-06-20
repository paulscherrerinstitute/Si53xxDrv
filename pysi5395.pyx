# cython: c_string_type=str, c_string_encoding=ascii
# ^^^^^^ DO NOT REMOVE THE ABOVE LINE ^^^^^^^

from libcpp.string cimport *
from libc.stdint   cimport *
from libc.stdio    cimport *

cdef extern from "Si5395.h" namespace "Si53xx":
  cdef cppclass Si53xx:
    get(const string &) except +

  cdef extern from "Si5395.h":
    cdef cppclass Si5395:
      ctypedef uint64_t ValType;
      Si5395() except+
      Si5395(const char *, unsigned) except+
      ValType  get(const string &)            except+
      void     set(const string &, ValType v) except+
      void     readCSV(const char *)          except+
      void     getNDivider(unsigned idx, ValType *num, ValType *den) except+
      void     setNDivider(unsigned idx, ValType num, ValType den) except+

      void     getPDivider(unsigned idx, ValType *num, ValType *den) except+
      void     setPDivider(unsigned idx, ValType num, ValType den) except+

      void     getMDivider( ValType *num, ValType *den) except+
      void     setMDivider(ValType num, ValType den) except+

      void     getMXAXBDivider( ValType *num, ValType *den) except+
      void     setMXAXBDivider(ValType num, ValType den) except+

      void     sendPreamble() except+
      void     sendPostamble() except+

cdef class SI5395:
    cdef Si5395 c_cls

    def __init__(self, busn=None, i2cAddr=0x68):
      if ( not busn is None ):
        self.c_cls = Si5395( busn, i2cAddr )
      else:
        self.c_cls = Si5395()

    def get(self, key):
      v = self.c_cls.get( key )
      return  v

    def set(self, key, val):
      self.c_cls.set( key, val )

    def readCSV(self, fnam):
      self.c_cls.readCSV( fnam )

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

    def setMDivider(self, num, den):
      self.c_cls.setMDivider(num, den)

    def sendPreamble(self):
      self.c_cls.sendPreamble()

    def sendPostamble(self):
      self.c_cls.sendPostamble()

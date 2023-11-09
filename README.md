# Driver for Si5395 PLL

This driver supports the Si5395 and similar PLL chips under (user-mode)
linux. The driver is written in C++ and provides python (using cython) as
well as EPICS-iocsh bindings.

Most functionality is implemented in a Si53xx base class from which the Si5395 class derives.
Similar devices may differ in the number of outputs they have and the range
of clock frequencies they support etc.

Unfortunately the available documentation is very limited. Many important
technical details apparently are a trade secret. Skyworks' technical "support"
declined even to provide basic information such as the VCO operating range.

No information is available about the actual loop configuration (phase-
detector operating principles, filter order and type, PLL capture
range, etc.). The user is referred to a windows-only software they are
supposed to use to configure the device. This obviously is very cumbersome,
difficult to maintain and inflexible.

From looking at the open-loop transfer function plotted by the tool
one might guess that the loop filter has three poles (one at zero)
and one zero which would make this a 4-th order type 2 PLL.

                   a       (s + b)
      Hfilt(s)=   ---  -------------
                   s   s^2 + c s + d

The way this driver works is that it is initialized to a base configuration
that was computed with the windows tool. This essentially fixes the PFD
frequency to a small range and defines the loop filter parameters. The
PFD range can be explored with the windows tool by scanning a range of input
frequencies (in ZDM mode) while keeping the bandwidth constant. Analyze the
design report verifying that the bandwidth registers remain constant and
take note of the PFD frequences.

With the base configuration in place the device can then be programmed
to generate desired frequencies just by changing the various dividers.

## I2C Access

For i2c access the driver requires the `i2c-dev` driver which implements
user-mode access to i2c via a `char` device.

## Initialization

A driver instance must be created providing the i2c bus number and device
address of the targeted Si5395:

     Si5395 *myDev = new Si5395( busNo, i2cAddr )
     string  designId;
     myDev->init( designId );

in python

     import pysi5395
     designId = ""
     myDev    = pysi5395.Si5395( busNo, i2cAddr, designId )

Note that the python wrapper calls the `init` method automatically.

The designId (a string) may be used to select one of multiple base-
configurations built into the driver. If the string is empty or
omitted then the base-configuration with the lexically biggest
design ID (supposedly: the 'latest version') is used.

## Basic Access

The driver implements basic `get` and `set` methods for all register
settings listed in the Si5395 reference manual (note that many settings
albeit listed are completely undocumented). These methods take a string 'key'
argument which identifies the setting and matches the documented name
of the setting. The returned value (`get`) or value argument (`set`) is
an integer number. E.g:

     myDev->get( "DESIGN_ID0" ); // return first char of design ID

Note that these basic access methods are intended for low-level
expert access and for building more specific functionality.

Whenever the PLL is touched for example these manipulations must be
preceded by a sequence of "preamble" operations and followed by a "postamble".
The basic acess methods *do not* respect this requirement and it is the
user's responsability to take care of the special commands.

By contrast, the more specialized operations, e.g., to program ZDM mode
check that the PLL is disabled (the result of a "preamble") and throws
and exception if no preamble had been sent.

### Caching

The driver implements a cache (for non-self-modifying settings only) for
more efficient access. This implies, however, that the device *must not*
be manipulated by other means (other driver instances, other i2c access
methods etc) because this would make the cache inconsistent.

## Dedicated Operations

Many operations exist to help programming the device; consult the `Si53xx.h`
header for details. As an example we show how the device can be programmed
for ZDM mode (assuming output OUT9A/IN3 for feedback).

     myDev->sendPreamble(); // disable PLL
     // set ZDM for desired frequency on input 2 using an R-divider of 8
     myDev->setZDM( freqHz, 2, 8, OutputConfig::LVDS33 );
     myDev->sendPostamble(); // enable PLL

Selecting a specific R divider for the ZDM output allows for defined
ratios on other outputs sharing the same N-divider as the ZDM output.

E.g., we can program output 8 to share the N divider with ZDM but
use a R divider of 4 and thus generating double the input frequency.

     myDev->setOutputMux( 8, 0 ); // let output 8 use N-divider 0
     myDev->setOutput( 8, OutputConfig::LVDS33, 4 );

In python:

     # the device is also a context manager that handles
     # the pre- and postamble
     with myDev as si:
       si.setZDM( freqHz, 2, 8, OutputConfig.LVDS33 )
     myDev.setOutputMux( 8, 0 )
     myDev.setOutput( 8, OutputConfig.LVDS33, 4 )

Note the the methods addressing outputs take an optional 'alt' argument
which defaults to `false` and is used to address the 'alternate' outputs
`OUT0A` and `OUT9A`. The reason for not renumbering the outputs is that
other device flavours may feature other 'alternate' outputs.

## Adding New Base Configurations

The driver supports multiple built-in base configurations. These are
created with the windows tool which must export a `Registers.h` C-style
header file. Make sure to use a unique design ID for the new configuration
because all available configurations are stored in a map indexed by
design-ID. Keep in mind that by default the driver uses the configuration
with the lexically biggest ID.

The new header file can simply be copied to the driver source directory
and added to git. The makefiles should take care of building the configuration
into the driver.

The configuration is loaded by the `init` method:

     myDev->init( forceReinit, "designId" );

The `forceReinit` argument is a boolean which allows you to reload
a configuration event if it is found to have prevously been loaded
already. If the new "designId" differs from the currently loaded design Id
then the reinitialization is always forced.

### Currently Available Base Configurations

There are currently two base configurations with different bandwidths;
both use a phase-detector frequency close to 1.9MHz.

 - Design `CIO0100` is a narrow-band design (loop bandwidth 82Hz).
 - Design `CIO0200` has a loop bandwidth of 680Hz.

The `CIO0200` configuration has been verified to maintain lock when
using a 500MHz reference which is FM-modulated to a max. deviation of
+/-10kHz with a trangular wave of a frequency of 0.007Hz.
This meets the SLS2 LLRF-500M-REQ specification (2023/01/27, Table 6:
CW ~500MHz, modulation less than 240Hz/s, tuning-range +/-10kHz).

## EPICS IOCSH Support

When built with the PSI EPICS build system then iocsh support is built
into the driver using the `iocshDeclWrapper.h` templates.

Note that not all functionality is mapped but it is straightforward
to add wrappers when needed.

### Driver Instantiation

In IOCSH a driver instance is created and bound to a Si5395 device
with

     Si5395RegisterClock name i2c-bus i2c-addr [designID]

The clock device must be registered with a unique but arbitrary name.
The i2c-bus is the linux device file name of the i2c bus where the
Si5395 is attached. Note that linux' `i2c-dev` driver must be loaded.
E.g.,

     Si5395RegisterClock Si5395_A /dev/i2c-5 0x68

Because iocsh has no support for anything like classes object access
is a bit clumsy:

     method_name object_name arguments...

the method names correspond to the c++ method names with "Si5395_"
prepended. The object name is the name used when registering the
device. E.g., to read the ID of the currently loaded base configuration:

     Si5395_getDesignId Si5395_A

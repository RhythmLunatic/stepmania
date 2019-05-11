list(APPEND SMDATA_IO_SRC
  "io/Python23IO.cpp"
  "io/USBDevice_Libusb.cpp"
  "io/USBDriver.cpp"
  "io/PIUIO.cpp"
)

list(APPEND SMDATA_IO_HPP
  "io/Python23IO.h"
  "io/USBDevice.h"
  "io/USBDriver.h"
  "io/PIUIO.h"
)


source_group("SM IO" FILES ${SMDATA_IO_SRC} ${SMDATA_IO_HPP})

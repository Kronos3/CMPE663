# CMPE 663

Real-time and Embedded Systems

## Usage

### First-time setup
Install an Arm GCC compiler with the following toolchain:

```bash
$ ls ~/arm-gcc
arm-none-eabi-addr2line  arm-none-eabi-elfedit    arm-none-eabi-gcc-ranlib     arm-none-eabi-gdb-add-index-py  arm-none-eabi-objcopy  arm-none-eabi-strip
arm-none-eabi-ar         arm-none-eabi-g++        arm-none-eabi-gcov           arm-none-eabi-gdb-py            arm-none-eabi-objdump
arm-none-eabi-as         arm-none-eabi-gcc        arm-none-eabi-gcov-dump      arm-none-eabi-gprof             arm-none-eabi-ranlib
arm-none-eabi-c++        arm-none-eabi-gcc-9.3.1  arm-none-eabi-gcov-tool      arm-none-eabi-ld                arm-none-eabi-readelf
arm-none-eabi-c++filt    arm-none-eabi-gcc-ar     arm-none-eabi-gdb            arm-none-eabi-ld.bfd            arm-none-eabi-size
arm-none-eabi-cpp        arm-none-eabi-gcc-nm     arm-none-eabi-gdb-add-index  arm-none-eabi-nm                arm-none-eabi-strings
```

Add to `$PATH` so that CMake may find these tools. See `CMakeLists.txt` for more information
on which tools will be used. Make sure all executables have `+x` permissions.

Add the following to `~/.bashrc` for ease of use:

```bash
usb-find() {
	for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
    (
        syspath="${sysdevpath%/dev}"
        devname="$(udevadm info -q name -p $syspath)"
        [[ "$devname" == "bus/"* ]] && exit
        eval "$(udevadm info -q property --export -p $syspath)"
        [[ -z "$ID_SERIAL" ]] && exit
        echo "/dev/$devname - $ID_SERIAL"
    )
	done
}
```

With the ST board plugged in, you should see it connected to a USB serial port:
```
$ usb-find
/dev/ttyACM0 - STMicroelectronics_STM32_STLink_0667FF544856846687075236
...
```

### General usage
The `master` branch will the basic CLion project setup required to compile and run the sources
you should branch off of master when creating a new project.

master is configured for the `STM32L467RG` board but the files may easily be regenerated.

### UART
To use the interface with the serial UART device on board the STM board,
use the `screen` tool to connect.

```bash
$ screen /dev/ttyACM0 BAUDRATE
```

Use `CTRL-A k` to kill the session.


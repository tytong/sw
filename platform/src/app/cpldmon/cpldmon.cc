/*
 * Copyright (C) 2019-2020 Pensando Systems Inc.
 *
 * Pensando CPLD Monitor (cpldmon)
 *
 */

#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <syslog.h>
#include "gpio.h"

#ifdef ELBA
#include "third-party/asic/elba/verif/apis/elb_freq_sw_api.h"
#include "platform/elba/csrint/csr_init.hpp"
#define  SET_HALF_CLOCK elb_top_set_half_core_clock_mode
#define  SET_QUARTER_CLOCK_MODE elb_top_set_quarter_core_clock_mode
#else
#include "third-party/asic/capri/verif/apis/cap_freq_api.h"
#include "platform/capri/csrint/csr_init.hpp"
#define  SET_HALF_CLOCK cap_top_set_half_clock
#define  SET_QUARTER_CLOCK_MODE cap_top_set_quarter_core_clock_mode
#endif
#include "nic/sdk/platform/pal/include/pal.h"
#include "nic/sdk/lib/pal/pal.hpp"
#include "nic/utils/trace/trace.hpp"
#include "nic/sdk/lib/logger/logger.hpp"
#include "nic/sdk/platform/pciemgr_if/include/pciemgr_if.hpp"

// CPLD to Capri gpio interrupt pin
#define CPLD_CAPRI_INT_GPIO    0

// CPLD interrupt enable/status bits
#define SFP_P1_PRESENT         0x01
#define SFP_P2_PRESENT         0x02
#define SFP_P1_REMOVE          0x04
#define SFP_P2_REMOVE          0x08
#define MARVELL                0x10
#define SFP_P1_ERROR           0x20
#define SFP_P2_ERROR           0x40
#define TEST_INTERRUPT         0x80

// CPLD extended interrupt enable/status bits
#define MAIN_POWER_ON          0x01
#define MAIN_POWER_OFF         0x02
#define TEMP_SENSOR_ALERT      0x04
#define TEMP_SENSOR_TRIP       0x08
#define OCP_PWRBRK             0x10

// CPLD control register bits
#define ALOM_PRESENT           0x20
#define HOST_POWER_ON          0x80

namespace cpldmon {

typedef std::shared_ptr<spdlog::logger> Logger;

// GetLogger returns a logger instance
Logger GetLogger() {
    static Logger _logger = spdlog::stdout_color_mt("cpldmon");
    static bool initDone = false;

    if (!initDone) {
        _logger->set_pattern("%L [%Y-%m-%d %H:%M:%S.%f] %P/%n: %v");
        initDone = true;
#ifdef DEBUG_ENABLE
        spdlog::set_level(spdlog::level::debug);
#endif
    }

    return _logger;
}

#define CLOG_INFO(args...) cpldmon::GetLogger()->info(args)
#define CLOG_WARN(args...) cpldmon::GetLogger()->warn(args)
#define CLOG_ERROR(args...) cpldmon::GetLogger()->error(args)
#define CLOG_FATAL(args...) { cpldmon::GetLogger()->error(args); assert(0); }

void initializeLogger();

} // namespace cpldmon

#define POWER  "/sys/class/hwmon/hwmon1/power1_input"

static int debug = 0;
static int dev_fd;

static int
cpldmon_exit(const char *msg, int ret)
{
    if (close(dev_fd) == -1)
        CLOG_ERROR("Failed to close /dev/gpiochip0");
    CLOG_ERROR("{} ({})", msg, ret);
    closelog();
    exit(ret);
}

static void
dump_cpld_int_enable(int int_enable)
{
    if (int_enable != -1) {
        CLOG_INFO("Interrupt enable reg 0x{}", int_enable);
        if (int_enable & SFP_P1_PRESENT)
            CLOG_INFO("SFP p1 present int enabled");
        if (int_enable & SFP_P2_PRESENT)
            CLOG_INFO("SFP p2 present int enabled");
        if (int_enable & SFP_P1_REMOVE)
            CLOG_INFO("SFP p1 remove int enabled");
        if (int_enable & SFP_P2_REMOVE)
            CLOG_INFO("SFP p2 remove int enabled");
        if (int_enable & MARVELL)
            CLOG_INFO("Marvell switch int enabled");
        if (int_enable & SFP_P1_ERROR)
            CLOG_INFO("SFP p1 error int enabled");
        if (int_enable & SFP_P2_ERROR)
            CLOG_INFO("SFP p2 error int enabled");
        if (int_enable & TEST_INTERRUPT)
            CLOG_INFO("Test int enabled");
    }
}

static void
dump_cpld_int_status(int int_status) {
    if (int_status != -1) {
        CLOG_INFO("Interrupt status register 0x{}", int_status);
        if (int_status & SFP_P1_PRESENT)
            CLOG_INFO("SFP p1 present int asserted");
        if (int_status & SFP_P2_PRESENT)
            CLOG_INFO("SFP p2 present int asserted");
        if (int_status & SFP_P1_REMOVE)
            CLOG_INFO("SFP p1 remove int asserted");
        if (int_status & SFP_P2_REMOVE)
            CLOG_INFO("SFP p2 remove int asserted");
        if (int_status & MARVELL)
            CLOG_INFO("Marvell switch int asserted");
        if (int_status & SFP_P1_ERROR)
            CLOG_INFO("SFP p1 error int asserted");
        if (int_status & SFP_P2_ERROR)
            CLOG_INFO("SFP p2 error int asserted");
        if (int_status & TEST_INTERRUPT)
            CLOG_INFO("Test int asserted");
    }
}

static void
dump_cpld_extended_int_enable(int ext_int_enable) {
    if (ext_int_enable != -1) {
        CLOG_INFO("Extended interrupt enable reg 0x{}", ext_int_enable);
        if (ext_int_enable & MAIN_POWER_ON)
            CLOG_INFO("Main power on int enabled");
        if (ext_int_enable & MAIN_POWER_OFF)
            CLOG_INFO("Main power off int enabled");
        if (ext_int_enable & TEMP_SENSOR_ALERT)
            CLOG_INFO("Temp sensor alert int enabled");
        if (ext_int_enable & TEMP_SENSOR_TRIP)
            CLOG_INFO("Temp sensor trip int enabled");
    }
}

static void
dump_cpld_extended_int_status(int ext_int_status) {
    if (ext_int_status != -1) {
        CLOG_INFO("Extended interrupt status reg 0x{}", ext_int_status);
        if (ext_int_status & MAIN_POWER_ON)
            CLOG_INFO("Main power on int asserted");
        if (ext_int_status & MAIN_POWER_OFF)
            CLOG_INFO("Main power off int asserted");
        if (ext_int_status & TEMP_SENSOR_ALERT)
            CLOG_INFO("Temp sensor alert int asserted");
        if (ext_int_status & TEMP_SENSOR_TRIP)
            CLOG_INFO("Temp sensor trip int asserted");
    }
}

static int
get_cpld_int_enable()
{
    int ret = cpld_reg_rd(CPLD_REGISTER_INTERRUPT_ENABLE);
    if (ret == -1)
        cpldmon_exit("Error reading cpld interrupt enable register", ret);
    if (debug)
        dump_cpld_int_enable(ret);
    return ret;
}

static int
get_cpld_int_status()
{
    int ret = cpld_reg_rd(CPLD_REGISTER_INTERRUPT_STATUS);
    if (ret == -1)
        cpldmon_exit("Error reading cpld interrupt status register", ret);
    if (debug)
        dump_cpld_int_status(ret);
    return ret;
}

static int
get_cpld_extended_int_enable()
{
    int ret = cpld_reg_rd(CPLD_REGISTER_EXT_INTERRUPT_ENABLE);
    if (ret == -1)
        cpldmon_exit("Error reading cpld extended interrupt enable register", ret);
    if (debug)
        dump_cpld_extended_int_enable(ret);
    return ret;
}

static int
get_cpld_extended_int_status()
{
    int ret = cpld_reg_rd(CPLD_REGISTER_EXT_INTERRUPT_STATUS);
    if (ret == -1)
        cpldmon_exit("Error reading cpld extended interrupt status register", ret);
    if (debug)
        dump_cpld_extended_int_status(ret);
    return ret;
}

static int
cpld_clear_enable_interrupt(uint8_t interrupt)
{
    int cpld_reg;
    uint8_t regval;
    int ret;

    // Clear
    cpld_reg = cpld_reg_rd(CPLD_REGISTER_INTERRUPT_ENABLE);
    if (cpld_reg == -1) {
        cpldmon_exit("Error reading cpld interrupt enable register", ret);
    } else {
        regval = cpld_reg & ~interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld interrupt enable register", ret);
    }
    // Enable
    regval = cpld_reg | interrupt;
    ret = cpld_reg_wr(CPLD_REGISTER_INTERRUPT_ENABLE, regval);
    if (ret == -1)
        cpldmon_exit("Error writing cpld interrupt enable register", ret);
    return 0;
}

static int
cpld_clear_enable_extended_interrupt(uint8_t interrupt)
{
    int cpld_reg;
    uint8_t regval;
    int ret;

    // Clear
    cpld_reg = cpld_reg_rd(CPLD_REGISTER_EXT_INTERRUPT_ENABLE);
    if (cpld_reg == -1) {
        cpldmon_exit("Error reading cpld extended interrupt enable register", ret);
    } else {
        regval = cpld_reg & ~interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_EXT_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld extended interrupt enable register", ret);
    }
    // Enable
    regval = cpld_reg | interrupt;
    ret = cpld_reg_wr(CPLD_REGISTER_EXT_INTERRUPT_ENABLE, regval);
    if (ret == -1)
        cpldmon_exit("Error writing cpld extended interrupt enable register", ret);
    return 0;
}

static int
cpld_enable_interrupt(uint8_t interrupt)
{
    int ret;
    uint8_t regval;

    ret = cpld_reg_rd(CPLD_REGISTER_INTERRUPT_ENABLE);
    if (ret == -1) {
        cpldmon_exit("Error reading cpld interrupt enable register", ret);
    } else {
        regval = ret | interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld interrupt enable register", ret);
        if (debug)
            CLOG_INFO("{}: Interrupt enable register 0x{}", __func__, regval);
    }
    return ret;
}

static int
cpld_disable_interrupt(uint8_t interrupt)
{
    int ret;
    uint8_t regval;

    ret = cpld_reg_rd(CPLD_REGISTER_INTERRUPT_ENABLE);
    if (ret == -1) {
        cpldmon_exit("Error reading cpld interrupt enable register", ret);
    } else {
        regval = ret & ~interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld interrupt enable register", ret);
        if (debug)
            CLOG_INFO("{}: Interrupt enable register 0x{}", __func__, regval);
    }
    return ret;
}

static int
cpld_enable_extended_interrupt(uint8_t interrupt)
{
    int ret;
    uint8_t regval;
   
    ret = cpld_reg_rd(CPLD_REGISTER_EXT_INTERRUPT_ENABLE);
    if (ret == -1) {
        cpldmon_exit("Error reading cpld extended interrupt register", ret);
    } else {
        regval = ret | interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_EXT_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld extended interrupt register", ret);
        if (debug)
            CLOG_INFO("{}: Extended interrupt enable register 0x{}", __func__, regval);
    }
    return ret;
}

static int
cpld_disable_extended_interrupt(uint8_t interrupt)
{
    int ret;
    uint8_t regval;

    ret = cpld_reg_rd(CPLD_REGISTER_EXT_INTERRUPT_ENABLE);
    if (ret == -1) {
        cpldmon_exit("Error reading cpld extended interrupt register", ret);
    } else {
        regval = ret & ~interrupt;
        ret = cpld_reg_wr(CPLD_REGISTER_EXT_INTERRUPT_ENABLE, regval);
        if (ret == -1)
            cpldmon_exit("Error writing cpld extended interrupt register", ret);
        if (debug)
            CLOG_INFO("{}: Extended interrupt enable register 0x{}", __func__, regval);
    }
    return ret;
}

static int 
get_card_power(float *power)
{
    unsigned long power_milliwatts;
    FILE *fd;

    fd = fopen(POWER, "r");
    if (fd < 0) {
        CLOG_INFO("Error opening sysfs hwmon1/power1_input ({})", -errno);
        return -1;
    }
    if (fscanf(fd, "%lu", &power_milliwatts) < 0) {
        CLOG_INFO("Error reading sysfs hwmon1/power1_input ({})", -errno);
        return -1;
    }
    *power = (float)power_milliwatts/1000000.0;
    return 0;
}

bool
naples25_swm(void) {
    int ret;

    ret = cpld_reg_rd(CPLD_REGISTER_ID);
    if (ret == -1)
        cpldmon_exit("Error reading cpld id register", ret);
    if (ret == CPLD_ID_NAPLES25_SWM)
        return true;
    return false;
}

bool
naples25_ocp(void) {
    int ret;

    ret = cpld_reg_rd(CPLD_REGISTER_ID);
    if (ret == -1)
        cpldmon_exit("Error reading cpld id register", ret);
    if (ret == CPLD_ID_NAPLES25_OCP)
        return true;
    return false;
}

static void 
usage(void)
{
    printf("cpldmon (-d) - Debug logging\n");
    printf("cpldmon (-h) - Help\n");
}

int
main(int argc, char *argv[])
{
    const char *device_name = "/dev/gpiochip0";
    uint32_t event_flags = GPIOEVENT_REQUEST_FALLING_EDGE;  // CPLD_CAPRI_INT_L
    uint32_t handle_flags = GPIOHANDLE_REQUEST_INPUT | 
                            GPIOHANDLE_REQUEST_OPEN_DRAIN;
    struct gpioevent_request req;
    struct gpioevent_data event;
    struct gpiohandle_data data;
    struct gpiochip_info cinfo;
    struct gpioline_info linfo;
    uint8_t int_status;
    uint8_t ext_int_status;
    uint16_t cpld_interrupt_cnt = 0;
    uint16_t sfp_p1_present_cnt = 0;
    uint16_t sfp_p2_present_cnt = 0;
    uint16_t sfp_p1_remove_cnt = 0;
    uint16_t sfp_p2_remove_cnt = 0;
    uint16_t marvell_cnt = 0;
    uint16_t sfp_p1_error_cnt = 0;
    uint16_t sfp_p2_error_cnt = 0;
    uint16_t test_interrupt_cnt = 0;
    uint16_t main_power_on_cnt = 0;
    uint16_t main_power_off_cnt = 0;
    uint16_t temp_sensor_alert_cnt = 0;
    uint16_t temp_sensor_trip_cnt = 0;
    uint16_t ocp_pwrbrk_cnt = 0;
    bool power_break = false;
    float card_power;
    int cpld_cntl_reg;
    int opt;
    int ret;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    struct option longopts[] = {
       { "debug", no_argument, NULL, 'd' },
       { "help",  no_argument, NULL, 'h' },
       { 0,       0,           0,     0 }
    };

    while ((opt = getopt_long(argc, argv, ":dh;", longopts, NULL)) != -1) {
        switch (opt) {
        case 'd':
            debug = 1;
            break;

        case 'h':
            usage();
            exit(0);
            break;

        default:
            fprintf(stderr, "%s: option -%c is invalid\n",
                    argv[0], optopt);
            usage();
            exit(1);
            break;
        }
    }

    // Operational details
    // - The CPLD interrupt enable bit needs to be set for valid status.
    // - CPLD read or write failure results in program exit.
    // - Restartable flag is set for this daemon in sysmgr.

    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    CLOG_INFO("STARTING");

    // Initialize pal
#ifdef __x86_64__
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_SIM) == sdk::lib::PAL_RET_OK);
#elif __aarch64__
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_HW) == sdk::lib::PAL_RET_OK);
#endif

#ifdef ELBA
    sdk::platform::elba::csr_init();
#else
    sdk::platform::capri::csr_init();
#endif

    // u-boot should have set the core clock to 208MHz for SWM/OCP
    if (get_card_power(&card_power) == 0)
        CLOG_INFO("Card power {} Watts", card_power);

    // Currently only Naples25 SWM and OCP cards have response timing
    // that requires use of the CPLD to Capri interrupt.
    ret = cpld_reg_rd(CPLD_REGISTER_ID);
    if (ret == -1) {
        cpldmon_exit("Error reading cpld id register", ret);
    } else {
        if (ret != CPLD_ID_NAPLES25_SWM && ret != CPLD_ID_NAPLES25_OCP) {
            CLOG_INFO("Not a Naples25 SWM or OCP card");
            while (1) {
                pause();
            }
        }
    }

    // Reboot after panic for single wire management cards otherwise
    // management over shared LOM will not recover without chassis power cycle.
    system("echo 1 > /sys/kernel/reboot/panic_reboot");

    // Check PCIe standup mode.  If CPLD indicates ALOM is present
    // check live status otherwise set the core clock to 416 MHz.
    // Single wire management cards are set to 208 MHz in u-boot.
    cpld_cntl_reg = cpld_reg_rd(CPLD_REGISTER_CTRL);
    if (cpld_cntl_reg == -1) {
        cpldmon_exit("Error reading cpld control register for ALOM presence", cpld_cntl_reg);
    } else {
        CLOG_INFO("cpld control register: 0x{}", cpld_cntl_reg);
        if (cpld_cntl_reg & ALOM_PRESENT) {
            CLOG_INFO("ALOM present");

            // Live status check
            if (cpld_cntl_reg & HOST_POWER_ON) {
                SET_HALF_CLOCK(0, 0);
                sleep(2);
                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("Main power is on, card power is now {} Watts", card_power);
            }
        } else {
            CLOG_INFO("ALOM not present, core clock set to 416 MHz");
            SET_HALF_CLOCK(0, 0);
            sleep(2);
            if (get_card_power(&card_power) == 0)
                CLOG_INFO("Card power {} Watts", card_power);
        }
    }

    // Configure CPLD to Capri GPIO interrupt
    dev_fd = open(device_name, 0);
    if (dev_fd == -1) {
        ret = -errno;
        cpldmon_exit("Failed to open /dev/gpiochip0", ret);
    }
    if (ioctl(dev_fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo) < 0) {
        CLOG_WARN("Unable to get gpiochip info");
    } else {
        CLOG_INFO("gpio chip: {}, \"{}\", {} GPIO lines",
                  cinfo.name, cinfo.label, cinfo.lines);
    }

    req.lineoffset = CPLD_CAPRI_INT_GPIO;
    req.handleflags = handle_flags;
    req.eventflags = event_flags;
    strcpy(req.consumer_label, "cpld-capri-gpio");

    ret = ioctl(dev_fd, GPIO_GET_LINEEVENT_IOCTL, &req);
    if (ret == -1) {
        cpldmon_exit("gpio get lineevent ioctl error", -errno);
    }

    /* Read initial states */
    ret = ioctl(req.fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &data);
    if (ret == -1) {
        ret = -errno;
        cpldmon_exit("gpiohandle get line values ioctl error", ret);
    }

    CLOG_INFO("Monitoring capri gpio line {} on {}",
              CPLD_CAPRI_INT_GPIO, device_name);

    // Clear and enable interrupts
    cpld_clear_enable_extended_interrupt(MAIN_POWER_ON);
    cpld_clear_enable_extended_interrupt(MAIN_POWER_OFF);
    CLOG_INFO("Host power on/off interrupts enabled");

    cpld_clear_enable_extended_interrupt(TEMP_SENSOR_ALERT);
    cpld_clear_enable_extended_interrupt(TEMP_SENSOR_TRIP);
    CLOG_INFO("Temperature alert and trip interrupts enabled");

    if (naples25_ocp()) {
        cpld_clear_enable_extended_interrupt(OCP_PWRBRK);
        CLOG_INFO("OCP power break interrupt enabled");
    }

    // Interrupt sources currently not used operationally
    //cpld_clear_enable_interrupt(SFP_P1_PRESENT);
    //cpld_clear_enable_interrupt(SFP_P2_PRESENT);
    //cpld_clear_enable_interrupt(SFP_P1_REMOVE);
    //cpld_clear_enable_interrupt(SFP_P2_REMOVE);
    //cpld_clear_enable_interrupt(MARVELL);
    //cpld_clear_enable_interrupt(SFP_P1_ERROR);
    //cpld_clear_enable_interrupt(SFP_P2_ERROR);
    //cpld_clear_enable_interrupt(TEST_INTERRUPT);

    // Initialize pciemgr object for power mode change notifications.
    class pciemgr *pciemgr = new class pciemgr("cpldmon");

    // Main loop responds to CPLD to Capri interrupt
    while (1) {
        ret = read(req.fd, &event, sizeof(event));
        if (ret == -1) {
            if (errno == -EAGAIN) {
                CLOG_WARN("No event");
                continue;
            } else {
                ret = -errno;
                CLOG_WARN("Error reading event ({})", ret);
                break;
            }
        }
        if (ret != sizeof(event)) {
            CLOG_WARN("Event size error ({})", ret);
            ret = -EIO;
            break;
        }

        switch (event.id) {
        case GPIOEVENT_EVENT_RISING_EDGE:
            if (debug)
                CLOG_INFO("CPLD rising edge interrupt");
            break;

        case GPIOEVENT_EVENT_FALLING_EDGE:
            if (debug)
                CLOG_INFO("CPLD falling edge interrupt ({})",
                          ++cpld_interrupt_cnt);

            // CPLD interrupt status bit check
            int_status = get_cpld_int_status();
            if (int_status & SFP_P1_PRESENT) {
                cpld_clear_enable_interrupt(SFP_P1_PRESENT);
                CLOG_INFO("SFP P1 present interrupt ({})", ++sfp_p1_present_cnt);
            }

            if (int_status & SFP_P2_PRESENT) {
                cpld_clear_enable_interrupt(SFP_P2_PRESENT);
                CLOG_INFO("SFP P2 present interrupt ({})", ++sfp_p2_present_cnt);
            }

            if (int_status & SFP_P1_REMOVE) {
                cpld_clear_enable_interrupt(SFP_P1_REMOVE);
                CLOG_INFO("SFP P1 removed interrupt ({})", ++sfp_p1_remove_cnt);
            }

            if (int_status & SFP_P2_REMOVE) {
                cpld_clear_enable_interrupt(SFP_P2_REMOVE);
                CLOG_INFO("SFP P2 removed interrupt ({})", ++sfp_p2_remove_cnt);
            }

            if (int_status & MARVELL) {
                cpld_clear_enable_interrupt(MARVELL);
                CLOG_INFO("Marvell switch interrupt ({})", ++marvell_cnt);
            }

            if (int_status & SFP_P1_ERROR) {
                cpld_clear_enable_interrupt(SFP_P1_ERROR);
                CLOG_INFO("SFP P1 error ({})", ++sfp_p1_error_cnt);
            }

            if (int_status & SFP_P2_ERROR) {
                cpld_clear_enable_interrupt(SFP_P2_ERROR);
                CLOG_INFO("SFP P2 error ({})", ++sfp_p2_error_cnt);
            }

            if (int_status & TEST_INTERRUPT) {
                // Toggle core clock between 208 and 416 MHz
                CLOG_INFO("Test interrupt ({})", ++test_interrupt_cnt);

                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("BEFORE: Card power {} Watts", card_power);

                if ((test_interrupt_cnt % 2) == 0)
                    SET_HALF_CLOCK(0, 0);
                else
                    SET_QUARTER_CLOCK_MODE(0, 0);

                sleep(2);
                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("AFTER: Card power {} Watts", card_power);
            }

            // CPLD extended interrupt status bit check
            ext_int_status = get_cpld_extended_int_status();

            if (power_break && !(ext_int_status & OCP_PWRBRK)) {
                CLOG_INFO("OCP clear power break");
                power_break = false;
            }

            if (ext_int_status & MAIN_POWER_ON) {
                CLOG_INFO("Main power on interrupt ({})", ++main_power_on_cnt);

                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("BEFORE: Card power {} Watts", card_power);

                SET_HALF_CLOCK(0, 0);
                pciemgr->powermode(FULL_POWER);
                cpld_clear_enable_extended_interrupt(MAIN_POWER_ON);

                sleep(2);
                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("AFTER: Card power {} Watts", card_power);
            }

            if (ext_int_status & MAIN_POWER_OFF) {
                CLOG_INFO("Main power off interrupt ({})", ++main_power_off_cnt);

                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("BEFORE: Card power {} Watts", card_power);

                pciemgr->powermode(LOW_POWER);
                SET_QUARTER_CLOCK_MODE(0, 0);
                cpld_clear_enable_extended_interrupt(MAIN_POWER_OFF);

                sleep(2);
                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("AFTER: Card power {} Watts", card_power);
            }

            if (ext_int_status & TEMP_SENSOR_ALERT) {
                cpld_clear_enable_extended_interrupt(TEMP_SENSOR_ALERT);
                CLOG_INFO("Temp sensor alert interrupt ({})", ++temp_sensor_alert_cnt);
            }

            if (ext_int_status & TEMP_SENSOR_TRIP) {
                cpld_clear_enable_extended_interrupt(TEMP_SENSOR_TRIP);
                CLOG_INFO("Temp sensor trip interrupt ({})", ++temp_sensor_trip_cnt);
            }

            if (ext_int_status & OCP_PWRBRK) {
                CLOG_INFO("OCP power break interrupt ({})", ++ocp_pwrbrk_cnt);

                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("BEFORE: Card power {} Watts", card_power);

                power_break = true;
                SET_QUARTER_CLOCK_MODE(0, 0);
                cpld_clear_enable_extended_interrupt(OCP_PWRBRK);

                sleep(2);
                if (get_card_power(&card_power) == 0)
                    CLOG_INFO("AFTER: Card power {} Watts", card_power);
            }
            break;
        default:
            CLOG_INFO("Unknown event (0x{})", event.id);
        }
    }
}

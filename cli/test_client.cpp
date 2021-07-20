#include <iostream>

#include <cli/cli.h>
#include <cli/clifilesession.h>

#include "linda-lib.hpp"

//ChipRegister values
#define DAC0 600
#define DAC1 2000
#define DAC2 2000u
#define DAC3 2000
#define DAC4 2000
#define DAC5 2000u
#define DACREF 300u
#define DACTEMP 32
#define GENPUR 0x230
#define TIMER_REG 8000u
#define TIMERSHUT 8000

using namespace cli;

int main(int argc, char* argv[]) {
    unsigned data[14400];
    std::string ip = "172.16.17.103";
    if(argc == 2) 
        ip = argv[1];

    if(InitCommunication(ip.c_str(), 32000, 32001, false) < 0)
        return -1;


        /*AcqInfo info{10,10,10,true,true,false};
        //ACQuisitionCont(info, data, 1<<0);
        ACQuisition(info, 1, 1<<0);
        PopFrame(data);
        for(int i = 0; i < 10; i++)
        	std::cout << data[i];
        std::cout << "\ndone\n"; */

    auto rootMenu = std::make_unique<Menu>("linda");

    rootMenu -> Insert(
            "config", [&](std::ostream& out, int chip) {
                unsigned ConfRegData[5]; 
                ConfRegData[4] =(DAC2<<22)|(DAC1<<11)|DAC0;
                ConfRegData[3] =(DAC5<<23)|(DAC4<<12)|(DAC3<<1)|(DAC2>>10);
                ConfRegData[2] = (DACREF<<24)|(DAC5>>9);
                ConfRegData[1] = (TIMER_REG<<30)|(GENPUR<<20)|(DACTEMP<<14)|(DACREF>>8);
                ConfRegData[0] = (TIMERSHUT<<12)|(TIMER_REG>>2);
                ChipRegisterWrite(ConfRegData, 1<<chip);
                out << "done\n"; 
        },
            "Config" 
    );

    rootMenu -> Insert(
            "start", [&](std::ostream& out, int bitmap_val) { 
                AcqInfo info{10,10,10,true,true,false};
                ACQuisitionCont(info, bitmap_val);
                out << "done\n"; 
        },
            "Start acquisition" 
    );

    rootMenu -> Insert(
            "start_2", [&](std::ostream& out, int bitmap_val, int frames) { 
                AcqInfo info{1000,4,2500,false,true,false};
                ACQuisition(info, frames, bitmap_val);
                out << "done\n"; 
        },
            "Start non continuous acquisition" 
    );

    rootMenu -> Insert(
            "pop_frame", [&](std::ostream& out) {
                PopData(data);
                for(int i = 0; i < 10; i++)
                    out << data[i];
                out << "\ndone\n"; 
        },
            "Pop frame" 
    );

    rootMenu -> Insert(
            "stop", [&](std::ostream& out) { 
                ACQuisitionStop();
                out << "done\n"; 
        },
            "Stop acquisition" 
    );

    rootMenu -> Insert(
            "read_id", [&](std::ostream& out, int chip) { 
                unsigned id;
                ReadEricaID(&id, 1<<chip);
                out << "Chip id: " << id << "\n"; 
        },
            "Read chip id" 
    );

    rootMenu -> Insert(
            "read_temp", [&](std::ostream& out, int chip) { 
                unsigned temp;
                ReadTemperature(&temp, 1<<chip);
                out << "ReadTemperature: " << temp << "\n"; 
        },
            "Read temperature" 
    );

    rootMenu -> Insert(
            "reset", [&](std::ostream& out) { 
                CameraReset();
                out << "done \n"; 
        },
            "Reset camera" 
    );

    rootMenu -> Insert(
            "chip_read", [&](std::ostream& out, int chip) {
                ChipRegisterRead(data, 1<<chip);
                out << "Done\n";
            },
            "Read chip register for a single chip"
    );

    rootMenu -> Insert(
            "pixel_read", [&](std::ostream& out, int chip) {
                PixelRegisterRead(data, 1<<chip);
                out << "Done\n";
            },
            "Read pixel register for a single chip"
    );

    rootMenu -> Insert(
            "reset_ctrlr", [&](std::ostream& out) { 
                ControllerReset();
                out << "done \n"; 
            },
            "Reset controller" 
    );

    rootMenu -> Insert(
            "reset_fb", [&](std::ostream& out) { 
                ResetBuffer();
                out << "done \n"; 
            },
            "Reset frame buffer" 
    );

    rootMenu -> Insert(
            "load_factors", [&](std::ostream& out) {
                LoadFloodNormFactors(data, 5);
            },
            "Load norm factors"
    );

    rootMenu -> Insert(
            "print_regs", [&](std::ostream& out) {
                PrintAllRegs();
            },
            "Print internal linda regs"
    );

    rootMenu -> Insert(
            "get_data_irqs", [&](std::ostream& out) {
                unsigned val;
                GetDataIRQs(&val);
                out << val << "\n";
            },
            "Print internal IRQs counter"
    );

    rootMenu -> Insert(
            "close", [&](std::ostream& out) {
                CloseCommunication();
                out << "Close executed\n";
            },
            "Close"
    );

    rootMenu -> Insert(
            "init", [&](std::ostream& out) {
                if(InitCommunication(ip.c_str(), 32000, 32001, true) < 0)
                    out << "Init error\n";
                out << "Init executed\n";
            },
            "Init"
    );


    Cli cli(std::move(rootMenu));
    CliFileSession input(cli);
    SetColor();
    input.Start();

    CloseCommunication();
}
    

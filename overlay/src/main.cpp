/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#define TESLA_INIT_IMPL
#include <exception_wrap.hpp>
#include <tesla.hpp>

#include "ui/gui/fatal_gui.h"
#include "ui/gui/main_gui.h"
#include "rgltr_services.h"  // for extern Service g_rgltrSrv, etc.

class AppOverlay : public tsl::Overlay
{
    public:
        AppOverlay() {}
        ~AppOverlay() {}

        //virtual void initServices() override {
        //    rgltrInitialize();
        //}

        virtual void exitServices() override {
            rgltrExit();
            sysclkIpcExit();
        }

        virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
        {
            uint32_t apiVersion;
            smInitialize();

            tsl::hlp::ScopeGuard smGuard([] { smExit(); });

            if(!sysclkIpcRunning())
            {
                return initially<FatalGui>(
                    "sys-clk未运行.\n\n"
                    "\n"
                    "请确保它已被正确地\n\n"
                    "安装并启用.",
                    ""
                );
            }

            if(R_FAILED(sysclkIpcInitialize()) || R_FAILED(sysclkIpcGetAPIVersion(&apiVersion)))
            {
                return initially<FatalGui>(
                    "无法连接到sys-clk.\n\n"
                    "\n"
                    "请确保所有组件\n\n"
                    "已正确安装并启用.",
                    ""
                );
            }

            //if(SYSCLK_IPC_API_VERSION != apiVersion)
            //{
            //    return initially<FatalGui>(
            //        "Overlay not compatible with\n\n"
            //        "the running sys-clk version.\n\n"
            //        "\n"
            //        "Please make sure everything is\n\n"
            //        "installed and up to date.",
            //        ""
            //    );
            //}

            return initially<MainGui>();
        }
};

int main(int argc, char **argv)
{
    return tsl::loop<AppOverlay>(argc, argv);
}

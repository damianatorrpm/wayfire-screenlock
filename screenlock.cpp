#include <wayfire/plugin.hpp>
#include <wayfire/output.hpp>
#include <wayfire/core.hpp>
#include <wayfire/view.hpp>
#include <wayfire/util/duration.hpp>
#include <wayfire/workspace-manager.hpp>
#include <wayfire/render-manager.hpp>
#include <wayfire/compositor-view.hpp>
#include <wayfire/output-layout.hpp>
#include <wayfire/debug.hpp>

#include <cmath>
#include <linux/input.h>
#include <wayfire/signal-definitions.hpp>

#include <string>
#include <sys/stat.h>

class wayfire_screenlock : public wf::plugin_interface_t
{
    wf::key_callback activate_binding;
    wf::wl_timer::callback_t on_timeout;
    wf::wl_timer timer;
    std::string path = "";
    int last_size;

    wf::option_wrapper_t<wf::keybinding_t> activate_key{"screenlock/activate"};
    wf::option_wrapper_t<std::string> cmd{"screenlock/command"};

    public:

    void init() override
    {
        grab_interface->name = "screenlock";
        grab_interface->capabilities = 0;

        using namespace std::placeholders;

	last_size = 0;
	if (getenv("XDG_RUNTIME_DIR") != NULL) {
            path = getenv("XDG_RUNTIME_DIR");
            path += "/screen-locker.trg";
	    last_size = get_size(path);
	}

	activate_binding = [=] (uint32_t)
	{
	    wf::get_core().run(((std::string) cmd).c_str());
	    return true;
	};
	output->add_key(activate_key, &activate_binding);

	on_timeout = [&] () {
	    timer.set_timeout(250, on_timeout);
	    int cur_size = get_size(path);
	    if (last_size != cur_size) {
		last_size = cur_size;
		wf::get_core().run(((std::string) cmd).c_str());
	    }
	};
	timer.set_timeout(250, on_timeout);
    }

    void fini() override
    {
	output->rem_binding(&activate_binding);
	timer.disconnect();
    }

private:
    int get_size(std::string path)
    {
	struct stat st;
	if (stat(path.c_str(), &st) == -1)
	    return 0;
	return st.st_size;
    }
};

DECLARE_WAYFIRE_PLUGIN(wayfire_screenlock);

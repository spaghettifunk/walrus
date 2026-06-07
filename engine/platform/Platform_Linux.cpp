#include <engine/platform/Platform.h>

// Linux platform layer.
#if WPLATFORM_LINUX

#include <engine/core/Logger.h>

#include <X11/XKBlib.h> // sudo apt-get install libx11-dev
#include <X11/Xlib-xcb.h> // sudo apt-get install libxkbcommon-x11-dev
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <sys/time.h>
#include <xcb/xcb.h>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h> // nanosleep
#else
#include <unistd.h> // usleep
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace Walrus
{
    struct Platform::PlatformState
    {
        Display* display = nullptr;
        xcb_connection_t* connection = nullptr;
        xcb_window_t window = 0;
        xcb_screen_t* screen = nullptr;
        xcb_atom_t wmProtocols = 0;
        xcb_atom_t wmDeleteWin = 0;
        bool initialized = false;
    };

    Platform::Platform(std::string applicationName, i32 x, i32 y, i32 width, i32 height)
        : m_State(std::make_unique<PlatformState>())
    {
        // Connect to X
        m_State->display = XOpenDisplay(NULL);
        if (!m_State->display)
        {
            WFATAL("Failed to open X display. Is DISPLAY set?");
            return;
        }

        // Turn off key repeats.
        XAutoRepeatOff(m_State->display);

        // Retrieve the connection from the display.
        m_State->connection = XGetXCBConnection(m_State->display);

        if (!m_State->connection || xcb_connection_has_error(m_State->connection))
        {
            WFATAL("Failed to connect to X server via XCB.");
            return;
        }

        // Get data from the X server
        const struct xcb_setup_t* setup = xcb_get_setup(m_State->connection);
        if (!setup)
        {
            WFATAL("Failed to get XCB setup data.");
            return;
        }

        // Loop through screens using iterator
        xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
        int screenP = 0;
        for (i32 s = screenP; s > 0; s--)
        {
            xcb_screen_next(&it);
        }

        // After screens have been looped through, assign it.
        m_State->screen = it.data;
        if (!m_State->screen)
        {
            WFATAL("Failed to get XCB screen.");
            return;
        }

        // Allocate a XID for the window to be created.
        m_State->window = xcb_generate_id(m_State->connection);

        // Register event types.
        // XCB_CW_BACK_PIXEL = filling then window bg with a single colour
        // XCB_CW_EVENT_MASK is required.
        u32 eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

        // Listen for keyboard and mouse buttons
        u32 eventValues = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_KEY_PRESS |
            XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_STRUCTURE_NOTIFY;

        // Values to be sent over XCB (bg colour, events)
        u32 valueList[] = { m_State->screen->black_pixel, eventValues };

        // Create the window
        xcb_void_cookie_t cookie = xcb_create_window(
            m_State->connection,
            XCB_COPY_FROM_PARENT, // depth
            m_State->window,
            m_State->screen->root, // parent
            x, // x
            y, // y
            width, // width
            height, // height
            0, // No border
            XCB_WINDOW_CLASS_INPUT_OUTPUT, // class
            m_State->screen->root_visual,
            eventMask,
            valueList);

        // Change the title
        xcb_change_property(
            m_State->connection,
            XCB_PROP_MODE_REPLACE,
            m_State->window,
            XCB_ATOM_WM_NAME,
            XCB_ATOM_STRING,
            8, // data should be viewed 8 bits at a time
            applicationName.size(),
            applicationName.c_str());

        // Tell the server to notify when the window manager
        // attempts to destroy the window.
        xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(m_State->connection, 0, strlen("WM_DELETE_WINDOW"), "WM_DELETE_WINDOW");
        xcb_intern_atom_cookie_t wmProtocolsCookie = xcb_intern_atom(m_State->connection, 0, strlen("WM_PROTOCOLS"), "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* wmDeleteReply = xcb_intern_atom_reply(m_State->connection, wmDeleteCookie, NULL);
        xcb_intern_atom_reply_t* wmProtocolsReply = xcb_intern_atom_reply(m_State->connection, wmProtocolsCookie, NULL);
        if (!wmDeleteReply || !wmProtocolsReply)
        {
            WFATAL("Failed to get window manager protocol atoms.");
            free(wmDeleteReply);
            free(wmProtocolsReply);
            return;
        }

        m_State->wmDeleteWin = wmDeleteReply->atom;
        m_State->wmProtocols = wmProtocolsReply->atom;

        xcb_change_property(
            m_State->connection, XCB_PROP_MODE_REPLACE, m_State->window, wmProtocolsReply->atom, 4, 32, 1, &wmDeleteReply->atom);

        free(wmDeleteReply);
        free(wmProtocolsReply);

        // Map the window to the screen
        xcb_map_window(m_State->connection, m_State->window);

        // Flush the stream
        i32 stream_result = xcb_flush(m_State->connection);
        if (stream_result <= 0)
        {
            WFATAL("An error occurred when flusing the stream: %d", stream_result);
            return;
        }

        m_State->initialized = true;
    }

    Platform::~Platform()
    {
        if (!m_State)
        {
            return;
        }

        if (m_State->display)
        {
            // Turn key repeats back on since this is global for the OS... just... wow.
            XAutoRepeatOn(m_State->display);
        }

        if (m_State->connection && m_State->window)
        {
            xcb_destroy_window(m_State->connection, m_State->window);
            xcb_flush(m_State->connection);
        }

        if (m_State->display)
        {
            XCloseDisplay(m_State->display);
        }
    }

    bool Platform::PumpMessages()
    {
        if (!m_State || !m_State->initialized)
        {
            return false;
        }

        xcb_generic_event_t* event;
        xcb_client_message_event_t* cm;

        bool quitFlagged = false;

        // Poll for events until null is returned.
        while ((event = xcb_poll_for_event(m_State->connection)))
        {
            // Input events
            switch (event->response_type & ~0x80)
            {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:
                {
                    // TODO: Key presses and releases
                }
                break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:
                {
                    // TODO: Mouse button presses and releases
                }
                break;
            case XCB_MOTION_NOTIFY:
                // TODO: mouse movement
                break;

            case XCB_CONFIGURE_NOTIFY:
                {
                    // TODO: Resizing
                }
                break;

            case XCB_CLIENT_MESSAGE:
                {
                    cm = (xcb_client_message_event_t*)event;

                    // Window close
                    if (cm->data.data32[0] == m_State->wmDeleteWin)
                    {
                        quitFlagged = true;
                    }
                }
                break;
            default:
                // Something else
                break;
            }

            free(event);
        }
        return !quitFlagged;
    }

    void* Platform::Allocate(u64 size, bool aligned)
    {
        (void)aligned;
        return malloc(size);
    }
    void Platform::Free(void* block, bool aligned)
    {
        (void)aligned;
        free(block);
    }
    void* Platform::ZeroMemory(void* block, u64 size)
    {
        return memset(block, 0, size);
    }
    void* Platform::CopyMemory(void* dest, const void* source, u64 size)
    {
        return memcpy(dest, source, size);
    }
    void* Platform::SetMemory(void* dest, i32 value, u64 size)
    {
        return memset(dest, value, size);
    }

    void Platform::ConsoleWrite(std::string message, u8 colour)
    {
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        std::string colourStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colourStrings[colour].c_str(), message.c_str());
    }
    void Platform::ConsoleWriteError(std::string message, u8 colour)
    {
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        std::string colourStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colourStrings[colour].c_str(), message.c_str());
    }

    double Platform::GetAbsoluteTime()
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return now.tv_sec + now.tv_nsec * 0.000000001;
    }

    void Platform::Sleep(u64 ms)
    {
#if _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000 * 1000;
        nanosleep(&ts, 0);
#else
        if (ms >= 1000)
        {
            sleep(ms / 1000);
        }
        usleep((ms % 1000) * 1000);
#endif
    }

} // namespace Walrus
#endif

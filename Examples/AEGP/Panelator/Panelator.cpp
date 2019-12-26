#include "Panelator.h"

#include "imgui.h"
#include "imgui_impl_osx.h"
#include "imgui_impl_opengl2.h"
#include "IconsFontAwesome5.h"

#import <Cocoa/Cocoa.h>
#include <new>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

template<>
const A_char *SuiteTraits<AEGP_PanelSuite1>::i_name = kAEGPPanelSuite;

template<>
const int32_t SuiteTraits<AEGP_PanelSuite1>::i_version = kAEGPPanelSuiteVersion1;

@interface ImGuiExampleView : NSOpenGLView
{
    NSTimer *animationTimer;
}
@end

@implementation ImGuiExampleView

- (void)animationTimerFired:(NSTimer *)timer
{
    [self setNeedsDisplay:YES];
}

- (void)reshape
{
    [[self openGLContext] update];
    // TODO
    //    [self updateAndDrawDemoView];
}

- (void)drawRect:(NSRect)bounds
{
    // TODO
    //    [self updateAndDrawDemoView];
}

- (void)dealloc
{
    animationTimer = nil;
}

- (void)keyUp:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)keyDown:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)flagsChanged:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseDown:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseUp:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseMoved:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)mouseDragged:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (void)scrollWheel:(NSEvent *)event
{
    ImGui_ImplOSX_HandleEvent(event, self);
}

- (BOOL)acceptsFirstResponder
{
    return (YES);
}

- (BOOL)becomeFirstResponder
{
    return (YES);
}

- (BOOL)resignFirstResponder
{
    return (YES);
}
@end

class Panelator
{
public:
    SPBasicSuite *m_pBasicSuite;
    SuiteHelper<AEGP_PanelSuite1> m_suiteHelper;
    AEGP_SuiteHandler m_suiteHandler;
    AEGP_PluginID m_pluginId;
    AEGP_Command m_command;
    const A_u_char *i_match_nameZ;
    ImGuiExampleView *m_pImguiView;

    static SPAPI A_Err CommandHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CommandRefcon refconP, AEGP_Command command,
                                   AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB)
    {
        PT_XTE_START
                {
                    reinterpret_cast<Panelator *>(plugin_refconP)->CommandHook(command, hook_priority, already_handledB, handledPB);
                }
        PT_XTE_CATCH_RETURN_ERR;
    }

    static A_Err UpdateMenuHook(AEGP_GlobalRefcon plugin_refconP, AEGP_UpdateMenuRefcon refconP, AEGP_WindowType active_window)
    {
        PT_XTE_START
                {
                    reinterpret_cast<Panelator *>(plugin_refconP)->UpdateMenuHook(active_window);
                }
        PT_XTE_CATCH_RETURN_ERR;
    }

    static A_Err CreatePanelHook(AEGP_GlobalRefcon plugin_refconP, AEGP_CreatePanelRefcon refconP, AEGP_PlatformViewRef container,
                                 AEGP_PanelH panelH, AEGP_PanelFunctions1 *outFunctionTable, AEGP_PanelRefcon *outRefcon)
    {
        PT_XTE_START
                {
                    reinterpret_cast<Panelator *>(plugin_refconP)->CreatePanelHook(container, panelH, outFunctionTable, outRefcon);
                }
        PT_XTE_CATCH_RETURN_ERR;
    }

    static A_Err CreateIdleHook(AEGP_GlobalRefcon plugin_refconP, AEGP_IdleRefcon refconP, A_long *max_sleepPL)
    {
        PT_XTE_START
                {
                    reinterpret_cast<Panelator *>(plugin_refconP)->IdleHook(plugin_refconP, refconP, max_sleepPL);
                }
        PT_XTE_CATCH_RETURN_ERR;
    }

    // TODO death hook

    Panelator(SPBasicSuite *pica_basicP, AEGP_PluginID pluginID) :
        m_pBasicSuite(pica_basicP),
        m_pluginId(pluginID),
        m_suiteHandler(pica_basicP),
        m_suiteHelper(pica_basicP),
        i_match_nameZ((A_u_char *) STR(StrID_Name)),
        m_pImguiView(NULL)
    {
        PT_ETX(m_suiteHandler.CommandSuite1()->AEGP_GetUniqueCommand(&m_command));
        PT_ETX(m_suiteHandler.CommandSuite1()->AEGP_InsertMenuCommand(m_command, STR(StrID_Name), AEGP_Menu_WINDOW, AEGP_MENU_INSERT_SORTED));
        PT_ETX(m_suiteHandler.RegisterSuite5()->AEGP_RegisterCommandHook(m_pluginId, AEGP_HP_BeforeAE, AEGP_Command_ALL, &Panelator::CommandHook, NULL));
        PT_ETX(m_suiteHandler.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(m_pluginId, &Panelator::UpdateMenuHook, NULL));
        PT_ETX(m_suiteHandler.RegisterSuite5()->AEGP_RegisterIdleHook(m_pluginId, &Panelator::CreateIdleHook, 0));
        PT_ETX(m_suiteHelper->AEGP_RegisterCreatePanelHook(m_pluginId, i_match_nameZ, &Panelator::CreatePanelHook, (AEGP_CreatePanelRefcon) this, true));
    }

    void CommandHook(AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean *handledPB)
    {
        if (command == m_command)
        {
            PT_ETX(m_suiteHelper->AEGP_ToggleVisibility(i_match_nameZ));
        }
    }

    void UpdateMenuHook(AEGP_WindowType active_window)
    {
        PT_ETX(m_suiteHandler.CommandSuite1()->AEGP_EnableCommand(m_command));

        A_Boolean out_thumb_is_shownB = FALSE;
        A_Boolean out_panel_is_frontmostB = FALSE;

        PT_ETX(m_suiteHelper->AEGP_IsShown(i_match_nameZ, &out_thumb_is_shownB, &out_panel_is_frontmostB));
        PT_ETX(m_suiteHandler.CommandSuite1()->AEGP_CheckMarkMenuCommand(m_command, out_thumb_is_shownB && out_panel_is_frontmostB));
    }

    void CreatePanelHook(
        AEGP_PlatformViewRef platformView,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1 *outFunctionTable,
        AEGP_PanelRefcon *outRefcon)
    {
        *outRefcon = reinterpret_cast<AEGP_PanelRefcon>(new PanelatorUI_Plat(m_pBasicSuite, panelH, platformView, outFunctionTable));

        NSView *pRootView = platformView;
        pRootView.autoresizesSubviews = true;

        GLint swapInterval = 1;

        NSOpenGLPixelFormatAttribute attrs[] =
            {
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFADepthSize, 32,
                0
            };
        NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
        m_pImguiView = [[ImGuiExampleView alloc] initWithFrame:pRootView.bounds pixelFormat:format];
        m_pImguiView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1070
        if (floor(NSAppKitVersionNumber) > NSAppKitVersionNumber10_6)
        {
            [m_pImguiView setWantsBestResolutionOpenGLSurface:YES];
        }
#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= 1070

        [pRootView addSubview:m_pImguiView];

        if ([m_pImguiView openGLContext] == nil)
        {
            NSLog(@"No OpenGL Context!");
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontDefault();

// merge in icons from Font Awesome
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        std::string fontPath = std::string(getcwd(NULL, 0)) + "/Plug-ins/fa-solid-900.ttf";

        io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, &icons_config, icons_ranges);

        ImGui::StyleColorsLight();
        ImGui_ImplOSX_Init();
        ImGui_ImplOpenGL2_Init();
    }

    A_Err IdleHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_IdleRefcon refconP,
        A_long *max_sleepPL)
    {
        A_Err err = A_Err_NONE;

        if (m_pImguiView != nullptr)
        {
            ImGui_ImplOpenGL2_NewFrame();
            ImGui_ImplOSX_NewFrame(m_pImguiView);
            ImGui::NewFrame();

            ShowNewWindow(ImVec2(m_pImguiView.bounds.size.width, m_pImguiView.bounds.size.height));
//            ShowDemoWindow();

            ImGui::Render();
            [[m_pImguiView openGLContext] makeCurrentContext];

            ImDrawData *draw_data = ImGui::GetDrawData();
            GLsizei width = (GLsizei) (draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
            GLsizei height = (GLsizei) (draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
            glViewport(0, 0, width, height);

            static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL2_RenderDrawData(draw_data);

            // Present
            [[m_pImguiView openGLContext] flushBuffer];
        }

        return err;
    }

    void ShowNewWindow(ImVec2 parentWindowSize)
    {
        static bool no_titlebar = true;
        static bool no_scrollbar = false;
        static bool no_menu = true;
        static bool no_move = true;
        static bool no_resize = true;
        static bool no_collapse = false;
        static bool no_close = true;
        static bool no_nav = false;
        static bool no_background = false;
        static bool no_bring_to_front = true;
        static bool open = true;

        ImGuiWindowFlags window_flags = 0;
        if (no_titlebar)
            window_flags |= ImGuiWindowFlags_NoTitleBar;
        if (no_scrollbar)
            window_flags |= ImGuiWindowFlags_NoScrollbar;
        if (!no_menu)
            window_flags |= ImGuiWindowFlags_MenuBar;
        if (no_move)
            window_flags |= ImGuiWindowFlags_NoMove;
        if (no_resize)
            window_flags |= ImGuiWindowFlags_NoResize;
        if (no_collapse)
            window_flags |= ImGuiWindowFlags_NoCollapse;
        if (no_nav)
            window_flags |= ImGuiWindowFlags_NoNav;
        if (no_background)
            window_flags |= ImGuiWindowFlags_NoBackground;
        if (no_bring_to_front)
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(parentWindowSize, ImGuiCond_Always);

        // Main body of the Demo window starts here.
        if (!ImGui::Begin("You.i Nexus", &open, window_flags))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        if (ImGui::CollapsingHeader("Design System  " ICON_FA_PENCIL_RULER))
        {
            ImGui::Indent(ImGui::GetStyle().FramePadding.x);

            if (ImGui::CollapsingHeader("Instructions  " ICON_FA_BOOK))
            {
                ImGui::Text("Instructions");
            }
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Colors  " ICON_FA_PALETTE))
            {
            }
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Typography  " ICON_FA_FONT))
            {
                ImGui::Text("Typography");
            }
            ImGui::Separator();
            ImGui::Unindent(ImGui::GetStyle().FramePadding.x);
        }

        if (ImGui::CollapsingHeader("Tools"))
        {
        }

        ImGui::End();
    }

    void ShowDemoWindow() const
    {
        static bool show_demo_window = true;
        if (show_demo_window)
        {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
    }
};

A_Err EntryPointFunc(struct SPBasicSuite *pica_basicP, A_long major_versionL, A_long minor_versionL,
                     AEGP_PluginID aegp_plugin_id, AEGP_GlobalRefcon *global_refconP)
{
    PT_XTE_START
            {
                *global_refconP = (AEGP_GlobalRefcon) new Panelator(pica_basicP, aegp_plugin_id);
            }
    PT_XTE_CATCH_RETURN_ERR;
}

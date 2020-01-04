#include "Panelator.h"

#include "imgui.h"
#include "imgui_impl_osx.h"
#include "imgui_impl_opengl2.h"
#include "IconsFontAwesome5.h"
#include "picojson.h"

// https://github.com/nlohmann/json
#import "json.hpp"

#include "designsystem.h"

using json = nlohmann::json;

#import <Cocoa/Cocoa.h>
#include <new>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include <stdio.h>
#include <iostream>
#include <fstream>


// https://fontawesome.com/icons?d=gallery&m=free

// Cloud based design
// https://assets.adobe.com/public/d608e206-7e47-43f4-5c1b-dacdcff45581



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
    json designSystemJsonObject;
    Youi::DesignSystem designSystem;

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
        NSFontManager *sharedFontManager = [NSFontManager sharedFontManager];
        NSArray<NSString *> *availableFontFamilies = [sharedFontManager availableFontFamilies];
        NSArray<NSString *> *availableFonts = [sharedFontManager availableFonts];
        
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

//        ImGui::StyleColorsLight();
        StyleColorsYouiLight();
        ImGui_ImplOSX_Init();
        ImGui_ImplOpenGL2_Init();

        std::fstream designSystemJsonFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");
        std::string designJson;
        designSystemJsonFile >> designJson;

        designSystem = nlohmann::json::parse(designJson);
    }

    void StyleColorsYouiLight()
    {
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowPadding.x = 5;
        style.WindowPadding.y = 5;
        style.FramePadding.x = 5;
        style.FramePadding.y = 5;
        style.ItemSpacing.x = 5;
        style.ItemSpacing.y = 5;
        style.ItemInnerSpacing.x = 5;
        style.ItemInnerSpacing.y = 5;
        style.TouchExtraPadding.x = 0;
        style.TouchExtraPadding.y = 0;
        style.IndentSpacing = 20;
        style.ScrollbarSize = 10;
        style.GrabMinSize = 5;

        style.WindowBorderSize = 0;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 1;
        style.TabBorderSize = 1;

        style.WindowRounding = 2;
        style.ChildRounding = 2;
        style.FrameRounding = 2;
        style.PopupRounding = 2;
        style.ScrollbarRounding = 2;
        style.GrabRounding = 2;
        style.TabRounding = 2;

        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.97f, 0.97f, 0.97f, 0.98f);
        colors[ImGuiCol_Border] = ImVec4(0.42f, 0.42f, 0.42f, 0.23f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.02f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.99f, 0.99f, 0.99f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.94f, 0.94f, 0.94f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.82f, 0.81f, 0.81f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.92f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.98f, 0.98f, 0.98f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.96f, 0.96f, 0.96f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.79f, 0.79f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.79f, 0.79f, 0.80f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.73f, 0.73f, 0.73f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.81f, 0.81f, 0.81f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.74f, 0.74f, 0.74f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.66f, 0.66f, 0.66f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.85f, 0.11f, 0.36f, 1.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.65f, 0.08f, 0.27f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.73f, 0.73f, 0.73f, 0.40f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
        colors[ImGuiCol_TabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.92f, 0.93f, 0.94f, 0.99f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.74f, 0.82f, 0.91f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.45f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.70f, 0.70f, 0.70f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
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

            ImGui::ShowDemoWindow();

//            ShowNewWindow(ImVec2(m_pImguiView.bounds.size.width, m_pImguiView.bounds.size.height));

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

    void UIDesignSystemInstructions()
    {
        if (ImGui::BeginTabItem(ICON_FA_PENCIL_RULER " Design"))
        {
            auto designsysteminstructions = designSystemJsonObject.find("designsysteminstructions");

            ImGui::EndTabItem();
        }
    }

    void UIColorPalette()
    {
        if (ImGui::BeginTabItem(ICON_FA_TINT " Colors"))
        {
            auto palette = designSystemJsonObject.find("palette");
            ImGuiColorEditFlags colorDisplayFlags = 0;
            colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayRGB;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHex;
            colorDisplayFlags |= ImGuiColorEditFlags_DisplayHSV;

            for (auto &flavor : palette->items())
            {
                std::string flavorName = flavor.key();

                if (flavorName != "instructions")
                {
                    ImGui::Text(flavorName.c_str(), "");

                    for (auto &colorSwatch : flavor.value())
                    {
//                    ImVec4 *color = &colorSwatch["colorRGBA"];
                        static ImVec4 color;
                        ImGui::ColorButton("colorSwatch", color, colorDisplayFlags, ImVec2(ImGui::GetFrameHeight() * 2, ImGui::GetFrameHeight() * 2));
                    }
                }
                else
                {
                    ImGui::Separator();
                    ImGui::Text(flavorName.c_str(), "");
                }
            }
            ImGui::EndTabItem();
        }
    }

    void UITypography()
    {
        if (ImGui::BeginTabItem(ICON_FA_FONT " Type"))
        {
            auto palette = designSystemJsonObject.find("typography");

            ImGui::EndTabItem();
        }
    }

    void UIMotionDesign()
    {
        if (ImGui::BeginTabItem(ICON_FA_FIGHTER_JET " Motion"))
        {
            auto motionSpec = designSystemJsonObject.find("motiondesign");

            ImGui::BeginTabBar("Design System!");
            {
                if (ImGui::BeginTabItem(" Macro"))
                {
                    auto macro = motionSpec->find("macro");
                    for (auto &element : *macro)
                    {
                        ImGui::Separator();
                        ImGui::BeginGroup();
                        {
                            ImGui::Indent(ImGui::GetStyle().IndentSpacing);
                            ImGui::TextUnformatted(element["name"].get<std::string>().c_str());
                            ImGui::TextUnformatted(element["description"].get<std::string>().c_str());
                            ImGui::TextUnformatted(element["element"].get<std::string>().c_str());
                            ImGui::TextUnformatted(element["transformation"].get<std::string>().c_str());
                            ImGui::TextUnformatted(element["duration"].get<std::string>().c_str());
                            ImGui::TextUnformatted(element["delay"].get<std::string>().c_str());

                            //                            auto curve = macro->find("curve");
                            ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
                        }
                        ImGui::EndGroup();
                    }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem(" Micro"))
                {
                    auto micro = motionSpec->find("micro");
                    for (auto &element : *micro)
                    {
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::EndTabItem();
        }
    }

    void ShowNewWindow(ImVec2 parentWindowSize)
    {
        ImGuiWindowFlags window_flags = 0;
        static bool open = true;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(parentWindowSize, ImGuiCond_Always);

        ImGui::Begin("Design System", &open, window_flags);
        {
            ImGui::BeginTabBar("Design System!");
            {
                UIDesignSystemInstructions();
                UIColorPalette();
                UITypography();
                UIMotionDesign();
            }
            ImGui::EndTabBar();
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

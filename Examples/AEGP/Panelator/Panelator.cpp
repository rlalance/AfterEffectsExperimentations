#include "Panelator.h"

#include "imgui.h"
#include "imgui_impl_osx.h"
#include "imgui_impl_opengl2.h"
#include "IconsFontAwesome5.h"
#include "picojson.h"
#import "imgui_internal.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
#import <list>
#import <codecvt>


// https://fontawesome.com/icons?d=gallery&m=free

// Cloud based design
// https://assets.adobe.com/public/d608e206-7e47-43f4-5c1b-dacdcff45581

// Simple helper function to load an image into a OpenGL texture with common settings
bool LoadTextureFromFile(const char *filename, GLuint *out_texture, int *out_width, int *out_height)
{
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

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
    bool bDesignSystemLoaded = false;
    Youi::DesignSystem designSystem;
    ImFont *pSymbolFont;
    ImFont *pBodyFont;
    ImFont *pH1Font;
    ImFont *pH2Font;
    GLuint vidMetalTexture;
    GLuint modMetalTexture;
    ImTextureID vidTextureID;
    ImTextureID modTextureID;

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

        AEGP_MemHandle resultMemH = NULL;
        m_suiteHandler.UtilitySuite6()->AEGP_GetPluginPaths(m_pluginId, AEGP_GetPathTypes_APP, &resultMemH);
        void **path;
        m_suiteHandler.MemorySuite1()->AEGP_LockMemHandle(resultMemH, reinterpret_cast<void **>(&path));
        const std::u16string AEApplicationPath(reinterpret_cast<const char16_t *>(path)); // will read until the null character
        m_suiteHandler.MemorySuite1()->AEGP_UnlockMemHandle(resultMemH);


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

        std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
        std::u16string fontPath = AEApplicationPath + u"/Plug-ins/Panelator.plugin/Contents/Resources/fonts/";
        std::u16string imagePath = AEApplicationPath + u"/Plug-ins/Panelator.plugin/Contents/Resources/images/";

        ImFontConfig mainFontConfig;
        mainFontConfig.FontDataOwnedByAtlas = true;
        mainFontConfig.MergeMode = false;

        std::string bodyFontPath = convert.to_bytes(fontPath + u"RobotoMono-Regular.ttf");
        pBodyFont = io.Fonts->AddFontFromFileTTF(bodyFontPath.c_str(), 18.0f, &mainFontConfig);

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        std::string symbolFontName = convert.to_bytes(fontPath + u"fa-solid-900.ttf");
        static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        pSymbolFont = io.Fonts->AddFontFromFileTTF(reinterpret_cast<const char *>(symbolFontName.c_str()), 16.0f, &icons_config, icons_ranges);

        std::string h1FontPath = convert.to_bytes(fontPath + u"RobotoMono-Regular.ttf");
        pH1Font = io.Fonts->AddFontFromFileTTF(h1FontPath.c_str(), 48.0f, &mainFontConfig);

        std::string h2FontPath = convert.to_bytes(fontPath + u"RobotoMono-Regular.ttf");
        pH2Font = io.Fonts->AddFontFromFileTTF(h1FontPath.c_str(), 18.0f, &mainFontConfig);

        int out_width;
        int out_height;

        std::string vidImagePath = convert.to_bytes(imagePath + u"vidsmall.png");
        LoadTextureFromFile(vidImagePath.c_str(), &vidMetalTexture, &out_width, &out_height);
        vidTextureID = (ImTextureID)vidMetalTexture;

        std::string modImagePath = convert.to_bytes(imagePath + u"modsmall.png");
        LoadTextureFromFile(modImagePath.c_str(), &modMetalTexture, &out_width, &out_height);
        modTextureID = (ImTextureID) modMetalTexture;

//        ImGui::StyleColorsDark();
//        ImGui::StyleColorsLight();
        StyleColorsYouiLight();

        ImGui_ImplOSX_Init();
        ImGui_ImplOpenGL2_Init();

        std::fstream designSystemJsonFile("/Users/richardlalancette/Desktop/DesignSystemV3.json");

        designSystem = nlohmann::json::parse(designSystemJsonFile);
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
            ImGui::PushFont(pBodyFont);

//            ImGui::ShowDemoWindow();
            ShowNewWindow(ImVec2(m_pImguiView.bounds.size.width, m_pImguiView.bounds.size.height));

            ImGui::PopFont();

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

    void StyleColorsYouiLight()
    {
        ImGuiStyle &style = ImGui::GetStyle();

        style.WindowPadding.x = 10;
        style.WindowPadding.y = 10;
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

    void UIDesignSystemInstructions()
    {
        if (ImGui::BeginTabItem(ICON_FA_PENCIL_RULER " Design"))
        {
            PageHeader("Design System", vidTextureID);

            auto ds = designSystem.instructions;

            for (const auto &m : *ds->metadata)
            {
                ImGui::TextWrapped("%s", m.c_str());
                ImGui::Separator();
            }

            ImGui::TextWrapped("Format: %s", ds->design_system_format->c_str());

            ImGui::EndTabItem();
        }
    }

    static void FlavorIdentifierDisplay(const std::string &flavorName)
    {
        // Orientation
        if (flavorName.find("landscape") != std::string::npos)
        {
            ImGui::Text(ICON_FA_IMAGE);
            ImGui::SameLine();
            ImGui::TextDisabled(" Landscape");
        }

        if (flavorName.find("portrait") != std::string::npos)
        {
            ImGui::Text(ICON_FA_PORTRAIT);
            ImGui::SameLine();
            ImGui::TextDisabled(" portrait");
        }

        // form factor
        if (flavorName.find("tv") != std::string::npos)
        {
            ImGui::Text(ICON_FA_TV);
            ImGui::SameLine();
            ImGui::TextDisabled(" tv");
        }

        if (flavorName.find("handset") != std::string::npos)
        {
            ImGui::Text(ICON_FA_MOBILE);
            ImGui::SameLine();
            ImGui::TextDisabled(" handset");
        }

        if (flavorName.find("tablet") != std::string::npos)
        {
            ImGui::Text(ICON_FA_TABLET);
            ImGui::SameLine();
            ImGui::TextDisabled(" tablet");
        }

        // Density
        if (flavorName.find("ldpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" ldpi (~120dpi) (0.75x)");
        }

        if (flavorName.find("mdpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" mdpi (~160dpi) (1.0x baseline)");
        }

        if (flavorName.find("hdpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" hdpi (~240dpi) (1.5x)");
        }

        if (flavorName.find("xhdpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" xhdpi (~320dpi) (2.0x)");
        }

        if (flavorName.find("xxhdpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" xxhdpi (~480dpi) (3.0x)");
        }

        if (flavorName.find("xxxhdpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" xxxhdpi (~640dpi) (4.0x)");
        }

        if (flavorName.find("nodpi") != std::string::npos)
        {
            ImGui::Text(ICON_FA_EYE);
            ImGui::SameLine();
            ImGui::TextDisabled(" nodpi (density-independent/no scaling)");
        }

        /*
            ldpi	Resources for low-density (ldpi) screens (~120dpi).
            mdpi	Resources for medium-density (mdpi) screens (~160dpi). (This is the baseline density.)
            hdpi	Resources for high-density (hdpi) screens (~240dpi).
            xhdpi	Resources for extra-high-density (xhdpi) screens (~320dpi).
            xxhdpi	Resources for extra-extra-high-density (xxhdpi) screens (~480dpi).
            xxxhdpi	Resources for extra-extra-extra-high-density (xxxhdpi) uses (~640dpi).
         */
    }

    void DetailedColorTooltip(const char *desc, const char *icon = "?", const char *flavorName = "")
    {
        ImGui::PushFont(pBodyFont);

        if (strlen(icon))
        {
            ImGui::TextDisabled(icon);
        }
        else
        {
            ImGui::TextDisabled(" ? ");
        }

        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushFont(pBodyFont);
            FlavorIdentifierDisplay(std::string(flavorName));
            ImGui::PopFont();
            ImGui::Separator();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 150.0f);
            ImGui::TextWrapped(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::PopFont();
    }

    void DetailsColorButton(const char *text, float col[4], ImGuiColorEditFlags flags)
    {
        ImGuiContext &g = *ImGui::GetCurrentContext();

        ImGui::Text(text);

        ImVec2 sz(g.FontSize * 3 + g.Style.FramePadding.y * 2, g.FontSize * 3 + g.Style.FramePadding.y * 2);
        ImVec4 cf(col[0], col[1], col[2], (flags & ImGuiColorEditFlags_NoAlpha) ? 1.0f : col[3]);
        int cr = IM_F32_TO_INT8_SAT(col[0]), cg = IM_F32_TO_INT8_SAT(col[1]), cb = IM_F32_TO_INT8_SAT(col[2]), ca = (flags & ImGuiColorEditFlags_NoAlpha) ? 255 : IM_F32_TO_INT8_SAT(col[3]);
        ImGui::ColorButton("##preview", cf, (flags & (ImGuiColorEditFlags__InputMask | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf)) | ImGuiColorEditFlags_NoTooltip, sz);
        ImGui::SameLine();
        if ((flags & ImGuiColorEditFlags_InputRGB) || !(flags & ImGuiColorEditFlags__InputMask))
        {
            if (flags & ImGuiColorEditFlags_NoAlpha)
                ImGui::Text("#%02X%02X%02X\nR: %d, G: %d, B: %d\n(%.3f, %.3f, %.3f)", cr, cg, cb, cr, cg, cb, col[0], col[1], col[2]);
            else
                ImGui::Text("#%02X%02X%02X%02X\nR:%d, G:%d, B:%d, A:%d\n(%.3f, %.3f, %.3f, %.3f)", cr, cg, cb, ca, cr, cg, cb, ca, col[0], col[1], col[2], col[3]);
        }
        else if (flags & ImGuiColorEditFlags_InputHSV)
        {
            if (flags & ImGuiColorEditFlags_NoAlpha)
                ImGui::Text("H: %.3f, S: %.3f, V: %.3f", col[0], col[1], col[2]);
            else
                ImGui::Text("H: %.3f, S: %.3f, V: %.3f, A: %.3f", col[0], col[1], col[2], col[3]);
        }
    }

    void UIColorPalette()
    {
        if (ImGui::BeginTabItem(ICON_FA_TINT " Colors"))
        {
            PageHeader("Colors", vidTextureID);

            auto colors = designSystem.colors;

            if (colors != nullptr)
            {
                auto flavors = colors->flavors;

                ImGuiColorEditFlags colorDisplayFlags = 0;
                colorDisplayFlags |= ImGuiColorEditFlags_AlphaPreviewHalf;
                colorDisplayFlags |= ImGuiColorEditFlags_DisplayRGB;
                colorDisplayFlags |= ImGuiColorEditFlags_DisplayHex;
                colorDisplayFlags |= ImGuiColorEditFlags_DisplayHSV;

                static bool alpha_preview = true;
                static bool alpha_half_preview = false;
                static bool drag_and_drop = true;
                static bool options_menu = true;
                static bool hdr = false;

                for (auto &flavor : *flavors)
                {
                    DetailedColorTooltip("Additional information and \nmetadata can be found here.", ICON_FA_INFO_CIRCLE, flavor.name->c_str());
                    ImGui::SameLine();
                    ImGui::PushFont(pH1Font);
                    ImGui::TextWrapped(flavor.name->c_str());

                    ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
                    ImGui::PopFont();

                    for (auto &colorSwatch : *flavor.flavor_colors)
                    {
                        auto colorRGBA = *colorSwatch.color_rgba;
                        auto colorName = colorSwatch.name->c_str();
                        float color[] = {static_cast<float>(*colorRGBA.r), static_cast<float>(*colorRGBA.g), static_cast<float>(*colorRGBA.b), static_cast<float>(*colorRGBA.a)};

                        DetailsColorButton(colorName, color, colorDisplayFlags);
                    }

                    ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
                    ImGui::Separator();
                    ImGui::Dummy(ImGui::GetStyle().ItemSpacing);
                }
            }
            else
            {
                ImGui::PushFont(pH1Font);
                ImGui::TextWrapped("No colors found in the design system.", "");
                ImGui::PopFont();
            }
            ImGui::EndTabItem();
        }
    }

    void PageHeader(const char *title, ImTextureID textureID)
    {
        ImVec2 currentPosition = ImGui::GetCursorPos();
        ImGui::Image(textureID, ImVec2(675, 51), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 0.85f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::SetCursorPos(currentPosition);

        ImGui::PushFont(pH1Font);
        const ImVec4 &white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        ImGui::Indent(ImGui::GetStyle().IndentSpacing);
        ImGui::TextColored(white, title);
        ImGui::Unindent();
        ImGui::PopFont();
        ImGui::Dummy(ImVec2(ImGui::GetStyle().IndentSpacing, ImGui::GetStyle().IndentSpacing));    }

    void UITypography()
    {
        if (ImGui::BeginTabItem(ICON_FA_FONT " Type"))
        {
            PageHeader("Typography", vidTextureID);
            ImGui::EndTabItem();
        }
    }

    void UIMotionDesign()
    {
        if (ImGui::BeginTabItem(ICON_FA_FIGHTER_JET " Motion"))
        {
            PageHeader("Motion", modTextureID);

            ImGui::BeginTabBar("Design System!");
            {
                if (ImGui::BeginTabItem(" Macro"))
                {
//                auto macro = motionSpec->find("macro");
//                for (auto &element : *macro)
//                {
//                    ImGui::Separator();
//                    ImGui::BeginGroup();
//                    {
//                        ImGui::Indent(ImGui::GetStyle().IndentSpacing);
//                        ImGui::TextUnformatted(element["name"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["description"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["element"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["transformation"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["duration"].get<std::string>().c_str());
//                        ImGui::TextUnformatted(element["delay"].get<std::string>().c_str());
//
//                        //                            auto curve = macro->find("curve");
//                        ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
//                    }
//                    ImGui::EndGroup();
//                }
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem(" Micro"))
                {
//                auto micro = motionSpec->find("micro");
//                for (auto &element : *micro)
//                {
//                }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::EndTabItem();
        }

//        if (ImGui::BeginTabItem(ICON_FA_FIGHTER_JET " Motion"))
//        {
//            auto motionSpec = designSystemJsonObject.find("motiondesign");
//
//            ImGui::BeginTabBar("Design System!");
//            {
//                if (ImGui::BeginTabItem(" Macro"))
//                {
//                    auto macro = motionSpec->find("macro");
//                    for (auto &element : *macro)
//                    {
//                        ImGui::Separator();
//                        ImGui::BeginGroup();
//                        {
//                            ImGui::Indent(ImGui::GetStyle().IndentSpacing);
//                            ImGui::TextUnformatted(element["name"].get<std::string>().c_str());
//                            ImGui::TextUnformatted(element["description"].get<std::string>().c_str());
//                            ImGui::TextUnformatted(element["element"].get<std::string>().c_str());
//                            ImGui::TextUnformatted(element["transformation"].get<std::string>().c_str());
//                            ImGui::TextUnformatted(element["duration"].get<std::string>().c_str());
//                            ImGui::TextUnformatted(element["delay"].get<std::string>().c_str());
//
//                            //                            auto curve = macro->find("curve");
//                            ImGui::Unindent(ImGui::GetStyle().IndentSpacing);
//                        }
//                        ImGui::EndGroup();
//                    }
//                    ImGui::EndTabItem();
//                }
//
//                if (ImGui::BeginTabItem(" Micro"))
//                {
//                    auto micro = motionSpec->find("micro");
//                    for (auto &element : *micro)
//                    {
//                    }
//                    ImGui::EndTabItem();
//                }
//                ImGui::EndTabBar();
//            }
//
//            ImGui::EndTabItem();
//        }
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
//            if (!bDesignSystemLoaded)
//            {
//                PageHeader("Design System", vidTextureID);
//
//                if (ImGui::Button("Load Design System File"))
//                {
//                    bDesignSystemLoaded = true;
//                }
//            }
//            else
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

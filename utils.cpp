#include "utils.h"
#include <iostream>

void draw_crosshair() {
    ImVec2 cp = ImGui::GetMousePos();

    ImDrawList* draw = ImGui::GetForegroundDrawList();
    draw->AddLine(ImVec2(cp.x, 0), ImVec2(cp.x, 1920), IM_COL32(0, 255, 0, 100), 1);
    draw->AddLine(ImVec2(0, cp.y), ImVec2(1600, cp.y), IM_COL32(0, 255, 0, 100), 1);
}
void UpdateRectangle(float& display_x1, float& display_y1, float& display_x2, float& display_y2) {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 mouse_pos = ImGui::GetMousePos();

    // Sensitivity for corner/edge grabbing
    const float edge_grab_threshold = 3.0f;

    // Check if we're hovering near any of the corners or edges
    bool near_left_edge = fabs(mouse_pos.x - display_x1) < edge_grab_threshold;
    bool near_right_edge = fabs(mouse_pos.x - display_x2) < edge_grab_threshold;
    bool near_top_edge = fabs(mouse_pos.y - display_y1) < edge_grab_threshold;
    bool near_bottom_edge = fabs(mouse_pos.y - display_y2) < edge_grab_threshold;

    // Check for corner grab
    bool near_top_left_corner = near_left_edge && near_top_edge;
    bool near_top_right_corner = near_right_edge && near_top_edge;
    bool near_bottom_left_corner = near_left_edge && near_bottom_edge;
    bool near_bottom_right_corner = near_right_edge && near_bottom_edge;

    // Edge grabbing state
    static bool is_dragging_edge = false;
    static ImVec2 drag_offset;

    // Handle mouse press
    if (!is_dragging_edge && ImGui::IsMouseDown(VK_LBUTTON)) {
        std::cout << "dragging edge" << std::endl;
        if (near_top_left_corner || near_top_right_corner || near_bottom_left_corner || near_bottom_right_corner) {
            is_dragging_edge = true;
            drag_offset = ImVec2(mouse_pos.x - display_x1, mouse_pos.y - display_y1);
        }
        else if (near_left_edge || near_right_edge || near_top_edge || near_bottom_edge) {
            is_dragging_edge = true;
            drag_offset = ImVec2(mouse_pos.x - display_x1, mouse_pos.y - display_y1);
        }
    }

    // Handle mouse release
    if (!ImGui::IsMouseDown(VK_LBUTTON)) {
        is_dragging_edge = false;
    }

    // Handle the actual edge dragging
    if (is_dragging_edge) {
        ImVec2 drag_delta = ImVec2(mouse_pos.x - drag_offset.x, mouse_pos.y - drag_offset.y);

        // Update the edges based on which part of the rectangle is being dragged
        if (near_top_left_corner) {
            display_x1 = drag_delta.x;
            display_y1 = drag_delta.y;
        }
        else if (near_top_right_corner) {
            display_x2 = drag_delta.x;
            display_y1 = drag_delta.y;
        }
        else if (near_bottom_left_corner) {
            display_x1 = drag_delta.x;
            display_y2 = drag_delta.y;
        }
        else if (near_bottom_right_corner) {
            display_x2 = drag_delta.x;
            display_y2 = drag_delta.y;
        }
        else if (near_left_edge) {
            display_x1 = drag_delta.x;
        }
        else if (near_right_edge) {
            display_x2 = drag_delta.x;
        }
        else if (near_top_edge) {
            display_y1 = drag_delta.y;
        }
        else if (near_bottom_edge) {
            display_y2 = drag_delta.y;
        }
    }
}
std::string utils::open_directory_dialog(const std::string& title) {
    // initialize com library
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        return "";
    }

    // create a file open dialog object
    IFileOpenDialog* file_dialog;
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&file_dialog));
    if (FAILED(hr)) {
        CoUninitialize();
        return "";
    }

    // set options for the dialog
    FILEOPENDIALOGOPTIONS options;
    hr = file_dialog->GetOptions(&options);
    if (SUCCEEDED(hr)) {
        options |= FOS_PICKFOLDERS;
        hr = file_dialog->SetOptions(options);
    }

    // set the dialog title
    std::wstring wide_title(title.begin(), title.end());
    hr = file_dialog->SetTitle(wide_title.c_str());
    if (FAILED(hr)) {
        file_dialog->Release();
        CoUninitialize();
        return "";
    }

    // show the dialog
    hr = file_dialog->Show(nullptr);
    if (FAILED(hr)) {
        file_dialog->Release();
        CoUninitialize();
        return "";
    }

    // get the selected item
    IShellItem* item;
    hr = file_dialog->GetResult(&item);
    if (FAILED(hr)) {
        file_dialog->Release();
        CoUninitialize();
        return "";
    }

    // get the path of the selected item
    PWSTR path;
    hr = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
    if (FAILED(hr)) {
        item->Release();
        file_dialog->Release();
        CoUninitialize();
        return "";
    }

    // convert the path to a string
    std::wstring wide_path(path);
    std::string result(wide_path.begin(), wide_path.end());

    // clean up
    CoTaskMemFree(path);
    item->Release();
    file_dialog->Release();
    CoUninitialize();

    return result;
}

#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <memory>
#include "LinuxForms/Application.h"
#include "InspectorField.h"

using namespace LinuxForms;

class Program : public LinuxForms::Application
{
public:
    void Initialize() override;
    void InitializeMenu();
    void CreateCallbacks();
    void LogMessage(const std::string& message);
    std::shared_ptr<Box> boxV;
    std::shared_ptr<Box> boxH;
    std::shared_ptr<Panel> panel1;
    std::shared_ptr<Panel> panel2;
    std::shared_ptr<Panel> panel3;
    std::shared_ptr<MenuBar> menubar;
    std::shared_ptr<DrawingArea> drawingarea1;
    std::shared_ptr<DrawingArea> drawingarea2;
    std::shared_ptr<Button> buttonCompile;
    std::shared_ptr<Button> buttonStop;
    std::shared_ptr<TextView> textviewLog;
    std::shared_ptr<ScrolledWindow> scrolledLog;
    std::shared_ptr<ScrolledWindow> scrolledInspector;
    std::shared_ptr<TabControl<SourceView>> tabcontrol;
    std::shared_ptr<SplitContainer> splitcontainer1;
    std::shared_ptr<SplitContainer> splitcontainer2;
    std::shared_ptr<Label> labelInspector;
private:
    //Menu Item Callbacks
    void OnMenuItemOpenClicked();
    void OnMenuItemSaveClicked();
    void OnMenuItemExitClicked();
    void OnMenuItemUndoClicked();
    void OnMenuItemRedoClicked();
    void OnMenuItemCutClicked();
    void OnMenuItemCopyClicked();
    void OnMenuItemPasteClicked();
    void OnMenuItemSelectAllClicked();
    void OnMenuItemClearLogClicked();
    void OnMenuItemMidiInitialize();

    void OnButtonCompileClicked(gpointer data);
    void OnButtonStopClicked(gpointer data);
    void OnDrawWave(GtkWidget* widget, cairo_t* cr, gpointer data);
    void OnDrawLevels(GtkWidget* widget, cairo_t* cr, gpointer data);
    void OnDrawingAreaClicked(guint button, double x, double y);
    void OnKeyDown(const KeyCode& keycode);
    void OnKeyUp(const KeyCode& keycode);

    static void OnCheckButtonToggled(GtkToggleButton *widget, gpointer user_data);
    static void OnEntryTextChanged(GtkEditable *editable, gpointer user_data);
    static void OnSpinButtonValueChanged(GtkSpinButton *spin_button, gpointer user_data);

    void OnApplicationQuit();

    void SetInspectorFields();

    //Other variables
    Color foregroundColor { 1.0f, 0.0f, 0.0f, 1.0f };
    Color backgroundColor { 1.0f, 0.0f, 0.0f, 1.0f };
    static std::vector<InspectorField> inspectorFields;

};

#endif

#include "Program.h"
#include <iostream>
#include "SharpAudioAPI.h"
#include "ByteMe.h"

std::vector<InspectorField> Program::inspectorFields;

void Program::Initialize()
{
    SharpAudioAPI::onDebug = [this](const std::string &message) { this->LogMessage(message); };

    boxV = std::make_shared<Box>(GtkOrientation::GTK_ORIENTATION_VERTICAL, 0, false);
    boxH = std::make_shared<Box>(GtkOrientation::GTK_ORIENTATION_HORIZONTAL, 0, false);
    panel1 = std::make_shared<Panel>(300, 64);
    panel2 = std::make_shared<Panel>(100, 40);
    panel3 = std::make_shared<Panel>(200, 100);
    buttonCompile = std::make_shared<Button>("Compile", 100, 20);
    buttonStop = std::make_shared<Button>("Stop", 100, 20);
    tabcontrol = std::make_shared<TabControl<SourceView>>();
    drawingarea1 = std::make_shared<DrawingArea>(128, 64);
    drawingarea2 = std::make_shared<DrawingArea>(128, 64);
    textviewLog = std::make_shared<TextView>();
    scrolledLog = std::make_shared<ScrolledWindow>();
    scrolledInspector = std::make_shared<ScrolledWindow>();
    splitcontainer1 = std::make_shared<SplitContainer>(GtkOrientation::GTK_ORIENTATION_HORIZONTAL);
    splitcontainer2 = std::make_shared<SplitContainer>(GtkOrientation::GTK_ORIENTATION_VERTICAL);
    labelInspector = std::make_shared<Label>("Inspector");

    InitializeMenu();

    window->Add(boxV->widget);
    boxV->Add(menubar->widget, false, false, 0);
    boxV->Add(panel1->widget, false, false, 0);
    panel1->Add(drawingarea1->widget, 0, 0);
    panel1->Add(drawingarea2->widget, 134, 0);
    boxV->Add(splitcontainer1->widget, true, true, 0);
    splitcontainer1->Add(splitcontainer2->widget, 0, true, false);
    splitcontainer1->Add(scrolledInspector->widget, 1, false, false);
    scrolledInspector->Add(panel3->widget);
    splitcontainer2->Add(tabcontrol->widget, 0, true, false);
    splitcontainer2->Add(boxH->widget, 1, false, false);
    boxH->Add(panel2->widget, false, false, 5);
    panel2->Add(buttonCompile->widget, 0, 0);
    panel2->Add(buttonStop->widget, 0, 30);
    boxH->Add(scrolledLog->widget, true, true, 0);
    scrolledLog->Add(textviewLog->widget);

    auto sourceview = tabcontrol->AddItem(true, true);
    sourceview->button->SetText("Main");

    if (IO::FileExists("res/KeyboardWurley.cs"))
    {
        std::string source = IO::ReadAllText("res/KeyboardWurley.cs");
        sourceview->item->SetText(source);
    }

    if (IO::FileExists("settings.bin"))
    {
        auto bytes = IO::ReadAllBytes("settings.bin");
        ByteStream stream(&bytes[0], bytes.size());
        foregroundColor.r = stream.Read<float>();
        foregroundColor.g = stream.Read<float>();
        foregroundColor.b = stream.Read<float>();
        foregroundColor.a = stream.Read<float>();
    }

    CreateCallbacks();

    window->SetStyleSheet("style.css");

    boxV->SetMargins(0, 5, 5, 5);
    drawingarea1->SetMargins(0, 0, 5, 0);
    drawingarea2->SetMargins(0, 0, 5, 0);

    window->SetSize({800, 600});
    window->SetTitle("SharpAudio");

    int height = window->rectangle.height;
    int width = window->rectangle.width;
    splitcontainer1->SetSeparatorPosition(width - 250);
    splitcontainer2->SetSeparatorPosition(height - 200);
    backgroundColor.SetFromHex(0x282828);

    textviewLog->SetReadOnly(true);

    window->Show();
}

void Program::InitializeMenu()
{
    MenuInfo menuInfoFile("File");
    MenuInfo menuInfoEdit("Edit");
    MenuInfo menuInfoView("View");
    MenuInfo menuInfoMidi("Midi");

    menuInfoFile.AddItem("Open", [this]() { this->OnMenuItemOpenClicked(); });
    menuInfoFile.AddItem("Save", [this]() { this->OnMenuItemSaveClicked(); });
    menuInfoFile.AddItem("Exit", [this]() { this->OnMenuItemExitClicked(); });

    menuInfoEdit.AddItem("Undo", [this]() { this->OnMenuItemUndoClicked(); });
    menuInfoEdit.AddItem("Redo", [this]() { this->OnMenuItemRedoClicked(); });
    menuInfoEdit.AddItem("Cut", [this]() { this->OnMenuItemCutClicked(); });
    menuInfoEdit.AddItem("Copy", [this]() { this->OnMenuItemCopyClicked(); });
    menuInfoEdit.AddItem("Paste", [this]() { this->OnMenuItemPasteClicked(); });
    menuInfoEdit.AddItem("Select All", [this]() { this->OnMenuItemSelectAllClicked(); });

    menuInfoView.AddItem("Clear Log", [this]() { this->OnMenuItemClearLogClicked(); });

    menuInfoMidi.AddItem("Initialize", [this]() { this->OnMenuItemMidiInitialize(); });

    std::vector<MenuInfo> menuInfo;
    menuInfo.push_back(menuInfoFile);
    menuInfo.push_back(menuInfoEdit);
    menuInfo.push_back(menuInfoView);
    menuInfo.push_back(menuInfoMidi);

    menubar = std::make_shared<MenuBar>(menuInfo);
}

void Program::CreateCallbacks()
{
    buttonCompile->onClicked += [this](gpointer data) { this->OnButtonCompileClicked(data); };
    buttonStop->onClicked += [this](gpointer data) { this->OnButtonStopClicked(data); };
    drawingarea1->onDraw += [this](GtkWidget *widget, cairo_t *cr, gpointer data) { this->OnDrawWave(widget, cr, data); };
    drawingarea2->onDraw += [this](GtkWidget *widget, cairo_t *cr, gpointer data) { this->OnDrawLevels(widget, cr, data); };
    drawingarea1->onButtonPressed += [this](guint button, double x, double y) { this->OnDrawingAreaClicked(button, x, y); };
    window->onClosing += [this]() { this->OnApplicationQuit(); };
}

void Program::LogMessage(const std::string &message)
{
    if (IsClosing()) //prevents setting text on a destroyed text buffer
        return;
    textviewLog->WriteLine(message);
}

void Program::OnButtonCompileClicked(gpointer data)
{
    auto sourceview = tabcontrol->GetSelectedItem();
    if (sourceview != nullptr)
    {
        std::string source = sourceview->GetText();
        if (SharpAudioAPI::Compile(source))
        {
            SetInspectorFields();
        }
    }
}

void Program::OnButtonStopClicked(gpointer data)
{
    SharpAudioAPI::StopScript();
}

void Program::OnDrawWave(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, backgroundColor.r, backgroundColor.g, backgroundColor.b);
    cairo_rectangle(cr, 0, 0, 128, 64);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, foregroundColor.r, foregroundColor.g, foregroundColor.b, foregroundColor.a);

    int count = SharpAudioAPI::GetAudioSourceCount();

    if (count > 0)
    {
        if (SharpAudioAPI::IsAudioSourcePlaying(0))
        {
            int pos = SharpAudioAPI::GetPlaybackPosition(0);
            long length = SharpAudioAPI::GetDataLength(0);
            long index = pos;
            if (index >= length)
            {
                index = (index - length);
            }

            double y = SharpAudioAPI::GetSampleAtOffset(0, index);
            y = y * 64;
            y += 32;
            cairo_move_to(cr, 0, y);

            for (int i = 0; i < 128; i++)
            {
                if (index >= length)
                {
                    index = (index - length);
                }

                y = SharpAudioAPI::GetSampleAtOffset(0, index);
                y = y * 64;
                y += 32;
                cairo_line_to(cr, i, y);
                index++;
            }

            cairo_stroke(cr);
        }
        else
        {
            cairo_move_to(cr, 0, 32);
            cairo_line_to(cr, 128, 32);
            cairo_stroke(cr);
        }
    }
    else
    {
        cairo_move_to(cr, 0, 32);
        cairo_line_to(cr, 128, 32);
        cairo_stroke(cr);
    }

    gtk_widget_queue_draw(widget);
}

void Program::OnDrawLevels(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    cairo_set_source_rgb(cr, foregroundColor.r, foregroundColor.g, foregroundColor.b);

    int count = SharpAudioAPI::GetAudioSourceCount();
    if (count > 0)
    {
        float left = SharpAudioAPI::GetSampleLeft(0);
        float right = SharpAudioAPI::GetSampleRight(0);
        cairo_rectangle(cr, 0, 0, left * 128, 31);
        cairo_fill(cr);
        cairo_rectangle(cr, 0, 32, right * 128, 31);
        cairo_fill(cr);
    }

    gtk_widget_queue_draw(widget);
}

void Program::OnDrawingAreaClicked(guint button, double x, double y)
{
    ColorPickerDialog dialog(window.get());

    if (dialog.ShowDialog(window.get()) == DialogResult::OK)
    {
        foregroundColor = dialog.GetColor();
    }
}

void Program::OnMenuItemOpenClicked()
{
    OpenFileDialog dialog;

    if (dialog.ShowDialog(window.get()) == DialogResult::OK)
    {
        std::string filename = dialog.GetFileName();

        if (IO::FileExists(filename))
        {
            std::string text = IO::ReadAllText(filename);
            auto tabPage = tabcontrol->AddItem();

            std::string tabname = StringUtility::GetFileNameWithExtension(filename);

            if (tabPage != nullptr)
            {
                tabPage->item->SetText(text);
                tabPage->button->SetText(tabname);
            }
        }
    }
}

void Program::OnMenuItemSaveClicked()
{
    SaveFileDialog dialog;

    if (dialog.ShowDialog(window.get()) == DialogResult::OK)
    {
        std::string filename = dialog.GetFileName();

        int selectedTab = tabcontrol->GetSelectedIndex();

        if (selectedTab >= 0)
        {
            auto selected = tabcontrol->GetItemAtIndex(selectedTab);

            if (selected != nullptr)
            {
                std::string text = selected->item->GetText();
                IO::WriteAllText(text, filename);
                Console::WriteLine("Saved file as " + filename);
            }
        }
    }
}

void Program::OnMenuItemExitClicked()
{
    Quit();
}

void Program::OnMenuItemUndoClicked()
{
    auto sourceview = tabcontrol->GetSelectedItem();

    if (sourceview != nullptr)
    {
        GtkSourceBuffer *buffer = sourceview->GetBuffer();

        if (gtk_source_buffer_can_undo(buffer))
            gtk_source_buffer_undo(buffer);
    }
}

void Program::OnMenuItemRedoClicked()
{
    auto sourceview = tabcontrol->GetSelectedItem();

    if (sourceview != nullptr)
    {
        GtkSourceBuffer *buffer = sourceview->GetBuffer();

        if (gtk_source_buffer_can_redo(buffer))
            gtk_source_buffer_redo(buffer);
    }
}

void Program::OnMenuItemCutClicked()
{
}

void Program::OnMenuItemCopyClicked()
{
}

void Program::OnMenuItemPasteClicked()
{
}

void Program::OnMenuItemSelectAllClicked()
{
    auto sourceview = tabcontrol->GetSelectedItem();

    if (sourceview != nullptr)
    {
        GtkSourceBuffer *buffer = sourceview->GetBuffer();
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(buffer), &start, &end);
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(buffer), &start, 0);
        gtk_text_buffer_get_iter_at_offset(GTK_TEXT_BUFFER(buffer), &end, 0);
        gtk_text_iter_forward_to_end(&end);
        gtk_text_buffer_select_range(GTK_TEXT_BUFFER(buffer), &start, &end);
    }
}

void Program::OnMenuItemClearLogClicked()
{
    textviewLog->Clear();
}

void Program::OnMenuItemMidiInitialize()
{
    SharpAudioAPI::InitializeMidi();
}

void Program::OnApplicationQuit()
{
    byte buffer[128];
    memset(buffer, 0, 128);
    ByteStream stream(buffer, 128);
    stream.Write(foregroundColor.r);
    stream.Write(foregroundColor.g);
    stream.Write(foregroundColor.b);
    stream.Write(foregroundColor.a);
    int length = stream.GetLength();

    IO::WriteAllBytes(buffer, length, "settings.bin");
}

void Program::SetInspectorFields()
{
    int numberOfFields = SharpAudioAPI::GetFieldCount();

    if (inspectorFields.size() > 0)
    {
        for (size_t i = 0; i < inspectorFields.size(); i++)
        {
            inspectorFields[i].Destroy();
        }
    }

    inspectorFields.clear();

    for (int i = 0; i < numberOfFields; i++)
    {
        int index = i;
        int fieldType = SharpAudioAPI::GetFieldType(index);

        std::string fieldName;
        SharpAudioAPI::GetFieldName(index, fieldName);

        std::string fieldValue;
        SharpAudioAPI::GetFieldValue(index, fieldValue);

        double fieldMinValue = SharpAudioAPI::GetFieldMinValue(index);
        double fieldMaxValue = SharpAudioAPI::GetFieldMinValue(index);

        InspectorField field;
        field.SetValueRange(fieldMinValue, fieldMaxValue);
        field.Create((FieldType)fieldType, panel3->widget, fieldName, index);
        FieldType ft = (FieldType)fieldType;
        field.Show();
        field.SetValue(fieldValue);
        inspectorFields.push_back(field);
    }

    for (int i = 0; i < numberOfFields; i++)
    {
        gpointer id = (gpointer)(&inspectorFields[i].identifier);

        switch (inspectorFields[i].type)
        {
            case FieldType::TypeBool:
            {
                g_signal_connect(GTK_TOGGLE_BUTTON(inspectorFields[i].inputWidget), "toggled", G_CALLBACK(OnCheckButtonToggled), id);
                break;
            }
            case FieldType::TypeDouble:
            {
                g_signal_connect(GTK_SPIN_BUTTON(inspectorFields[i].inputWidget), "value-changed", G_CALLBACK(OnSpinButtonValueChanged), id);
                break;
            }
            case FieldType::TypeFloat:
            {
                g_signal_connect(GTK_SPIN_BUTTON(inspectorFields[i].inputWidget), "value-changed", G_CALLBACK(OnSpinButtonValueChanged), id);
                break;
            }
            case FieldType::TypeInt:
            {
                g_signal_connect(GTK_SPIN_BUTTON(inspectorFields[i].inputWidget), "value-changed", G_CALLBACK(OnSpinButtonValueChanged), id);
                break;
            }
            case FieldType::TypeString:
            {
                g_signal_connect(GTK_ENTRY(inspectorFields[i].inputWidget), "changed", G_CALLBACK(OnEntryTextChanged), id);
                break;
            }
        }
    }
}

void Program::OnCheckButtonToggled(GtkToggleButton *widget, gpointer user_data)
{
    FieldIdentifier *identifier = (FieldIdentifier *)user_data;
    int id = identifier->id;
    std::string newValue = inspectorFields[id].GetValue();

    SharpAudioAPI::SetFieldValue(id, newValue);
}

void Program::OnEntryTextChanged(GtkEditable *editable, gpointer user_data)
{
    FieldIdentifier *identifier = (FieldIdentifier *)user_data;
    int id = identifier->id;
    std::string newValue = inspectorFields[id].GetValue();

    SharpAudioAPI::SetFieldValue(id, newValue);
}

void Program::OnSpinButtonValueChanged(GtkSpinButton *spin_button, gpointer user_data)
{
    FieldIdentifier *identifier = (FieldIdentifier *)user_data;
    int id = identifier->id;
    std::string newValue = inspectorFields[id].GetValue();

    SharpAudioAPI::SetFieldValue(id, newValue);
}

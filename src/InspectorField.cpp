#include "InspectorField.h"
#include "LinuxForms/Utility/Mathf.h"

void InspectorField::Create(FieldType type, GtkWidget *parent, std::string text, int index)
{
    identifier.id = index;
    this->type = type;
    int y = 30 + (index * 30);

    int textWidth = text.size() * 10;

    labelWidget = gtk_label_new(text.c_str());
    gtk_fixed_put(GTK_FIXED(parent), labelWidget, 5, y);

    if(type == FieldType::TypeBool)
    {
        inputWidget = gtk_check_button_new();
        gtk_fixed_put(GTK_FIXED(parent), inputWidget, 120, y);
    }
    else if(type == FieldType::TypeDouble)
    {
        if(minValue == maxValue)
            inputWidget = gtk_spin_button_new_with_range(0, 20000, 0.01);
        else
            inputWidget = gtk_spin_button_new_with_range(minValue, maxValue, 0.01);
        gtk_fixed_put(GTK_FIXED(parent), inputWidget, 120, y);
    }
    else if(type == FieldType::TypeFloat)
    {
        if(minValue == maxValue)
            inputWidget = gtk_spin_button_new_with_range(0, 20000, 0.01);
        else
            inputWidget = gtk_spin_button_new_with_range(minValue, maxValue, 0.01);
        gtk_fixed_put(GTK_FIXED(parent), inputWidget, 120, y);
    }
    else if(type == FieldType::TypeInt)
    {
        if(minValue == maxValue)
            inputWidget = gtk_spin_button_new_with_range(0, 20000, 1);
        else
            inputWidget = gtk_spin_button_new_with_range(minValue, maxValue, 1);
        gtk_fixed_put(GTK_FIXED(parent), inputWidget, 120, y);
    }    
    else
    {
        inputWidget = gtk_entry_new();
        entryBuffer = gtk_entry_get_buffer(GTK_ENTRY(inputWidget));
        gtk_fixed_put(GTK_FIXED(parent), inputWidget, 120, y);
    }    

}

void InspectorField::SetValue(std::string val)
{    
    switch(type)
    {
        case FieldType::TypeBool:
        {
            if(val.compare("True") == 0)
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputWidget), true);
            }
            else
            {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(inputWidget), false);
            }            
            break;
        }
        case FieldType::TypeDouble:
        {
            double v = 0;
            if(LinuxForms::Mathf::TryParseDouble(val, v))
            {
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(inputWidget), v);                
            }            
            break;
        }
        case FieldType::TypeFloat:
        {
            float v = 0;
            if(LinuxForms::Mathf::TryParseFloat(val, v))
            {
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(inputWidget), v);
            }
            break;
        }        
        case FieldType::TypeInt:
        {
            int v = 0;
            if(LinuxForms::Mathf::TryParseInt(val, v))
            {
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(inputWidget), v);
            }            
            break;
        }        
        case FieldType::TypeString:
        {
            gtk_entry_buffer_set_text(entryBuffer, val.c_str(), val.length());
            break;
        }
    }
}

void InspectorField::SetValueRange(double min, double max)
{
    this->minValue = min;
    this->maxValue = max;
}

std::string InspectorField::GetValue()
{
    switch(type)
    {
        case FieldType::TypeBool:
        {
            bool state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(inputWidget));
            std::string str = std::to_string(state);
            return str;
        }
        case FieldType::TypeDouble:
        {
            double v = gtk_spin_button_get_value(GTK_SPIN_BUTTON(inputWidget));
            return std::to_string(v);
        }
        case FieldType::TypeFloat:
        {
            double v = gtk_spin_button_get_value(GTK_SPIN_BUTTON(inputWidget));
            return std::to_string(v);
        }        
        case FieldType::TypeInt:
        {
            double v = gtk_spin_button_get_value(GTK_SPIN_BUTTON(inputWidget));
            int val = static_cast<int>(v);
            return std::to_string(val);
        }        
        case FieldType::TypeString:
        {
            const char * text = gtk_entry_buffer_get_text(entryBuffer);
            std::string str(text);
            return str;
        }
    }
    return std::string("");
}

void InspectorField::Show()
{
    gtk_widget_show(labelWidget);
    gtk_widget_show(inputWidget);    
}

void InspectorField::Destroy()
{
    gtk_widget_destroy(inputWidget);
    gtk_widget_destroy(labelWidget);
}

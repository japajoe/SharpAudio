#ifndef INSPECTORFIELD_HPP
#define INSPECTORFIELD_HPP

#include <gtk/gtk.h>
#include <string>

enum class FieldType
{
    TypeInt = 0,
    TypeFloat = 1,
    TypeDouble = 2,
    TypeBool = 3,
    TypeString = 4
};

typedef struct { int id; } FieldIdentifier;

class InspectorField
{
public:
    GtkWidget *labelWidget;
    GtkWidget *inputWidget;
    GtkEntryBuffer *entryBuffer;
    FieldIdentifier identifier;
    FieldType type;
    double minValue;
    double maxValue;

    void Create(FieldType type, GtkWidget *parent, std::string text, int index);
    std::string GetValue();
    void SetValue(std::string val);
    void SetValueRange(double min, double max);
    void Show();
    void Destroy();
};

#endif
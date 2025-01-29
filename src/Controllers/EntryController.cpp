#include "EntryController.h"

EntryController::EntryController(IView& display,
                                 IInput& input,
                                 HorizontalSelector& horizontalSelector,
                                 VerticalSelector& verticalSelector,
                                 FieldActionSelector& fieldActionSelector,
                                 ConfirmationSelector& confirmationSelector,
                                 StringPromptSelector& stringPromptSelector,
                                 EntryService& entryService,
                                 UsbService& usbService,
                                 LedService& ledService,
                                 NvsService& nvsService,
                                 ModelTransformer& modelTransformer)
    : display(display),
      input(input),
      horizontalSelector(horizontalSelector),
      verticalSelector(verticalSelector),
      fieldActionSelector(fieldActionSelector),
      confirmationSelector(confirmationSelector),
      stringPromptSelector(stringPromptSelector),
      entryService(entryService),
      usbService(usbService),
      ledService(ledService),
      nvsService(nvsService),
      modelTransformer(modelTransformer) {}

ActionEnum EntryController::actionFieldSelected(Field field) {
    auto action = fieldActionSelector.select(field.getLabel(), field.getValue());

    return action;
}

Entry EntryController::handleEntrySelection() {
    auto entries = entryService.getAllEntries();
    auto labels = modelTransformer.toStrings(entries);
    auto selectedIndex = verticalSelector.select(entryService.getVaultName(), labels, true, true);

    if (selectedIndex == -1) {
        return entryService.getEmptyEntry();
    }

    return entries[selectedIndex];
}

Field EntryController::handleFieldSelection(Entry& selectedEntry) {
    auto fieldValues = modelTransformer.toStrings(selectedEntry);
    std::vector<std::string> fieldLabels = {"User", "Pass", "Note"};
    std::vector<std::string> shortcuts = {"U", "P", "N"};
    auto selectedIndex = verticalSelector.select(selectedEntry.getServiceName(), fieldValues, true, false, fieldLabels, shortcuts, true);

    if (selectedIndex == -1) {
        return Field();
    }

    return Field(fieldLabels[selectedIndex], fieldValues[selectedIndex], shortcuts[selectedIndex]);
}

bool EntryController::handleEntryCreation() {
    auto servicerName = stringPromptSelector.select("New Entry", "Enter service name", "", true, false, true);
    if (servicerName.empty()) {
        return false;
    }

    auto username = stringPromptSelector.select("Username", "Enter username", "", false, true, true);
    auto password = stringPromptSelector.select("Password", "Enter password", "", false, true, true);
    auto notes = stringPromptSelector.select("Notes", "Enter notes (OK to pass)", "", false, true, true);

    auto entry = Entry(servicerName, username, password, notes);
    entryService.addEntry(entry);
    return true;
}

bool EntryController::handleEntryUpdate(Entry& entry, Field& field) {
    auto value = stringPromptSelector.select(field.getLabel(), "Modify the value", field.getValue(), true, true);
    if (value.empty()) {
        return false;
    }

    field.setValue(value);
    entryService.updateField(entry, field);
    display.subMessage("Field updated", 2000);
    return true;
}

bool EntryController::handleEntryDeletion() {
    size_t selectedIndex;
    bool entryUpdated = false;

    while (selectedIndex != 1) {
        auto entries = entryService.getAllEntries();
        auto labels = modelTransformer.toStrings(entries);
        std::vector<std::string> delLabels(labels.size(), "Del");
        auto selectedIndex = verticalSelector.select("Select to remove", labels, true, true, delLabels);
        if (selectedIndex == -1) {break;}
        auto confirmation = confirmationSelector.select("Erase Password", "Delete " + labels[selectedIndex] + " ?");
        if (confirmation) {
            entryService.deleteEntry(entries[selectedIndex]);
            entryUpdated = true;
        }
    }

    return entryUpdated;
}

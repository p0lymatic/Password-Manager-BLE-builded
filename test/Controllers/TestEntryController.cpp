#ifndef TEST_ENTRY_CONTROLLER
#define TEST_ENTRY_CONTROLLER

#include <unity.h>
#include "../src/Controllers/EntryController.h"
#include "../src/Services/EntryService.h"
#include "../src/Repositories/EntryRepository.h"
#include "../src/Transformers/ModelTransformer.h"
#include "../src/Selectors/StringPromptSelector.h"
#include "../src/Selectors/VerticalSelector.h"
#include "../src/Selectors/HorizontalSelector.h"
#include "../src/Selectors/FieldActionSelector.h"
#include "../src/Selectors/ConfirmationSelector.h"
#include "../src/Managers/InactivityManager.h"
#include "../Views/MockView.h"
#include "../Inputs/MockInput.h"

void test_handleEntryCreation() {
    MockView mockDisplay;
    MockInput mockInput;
    EntryRepository entryRepository;
    EntryService entryService(entryRepository);
    UsbService usbService;
    LedService ledService;
    NvsService nvsService;
    ModelTransformer modelTransformer;
    InactivityManager inactivityManager(mockDisplay);

    StringPromptSelector stringPromptSelector(mockDisplay, mockInput);
    VerticalSelector verticalSelector(mockDisplay, mockInput, inactivityManager);
    HorizontalSelector horizontalSelector(mockDisplay, mockInput, inactivityManager);
    FieldActionSelector fieldActionSelector(mockDisplay, mockInput, inactivityManager);
    ConfirmationSelector confirmationSelector(mockDisplay, mockInput);

    EntryController controller(mockDisplay, mockInput, horizontalSelector, verticalSelector,
                               fieldActionSelector, confirmationSelector, stringPromptSelector,
                               entryService, usbService, ledService, nvsService, modelTransformer);

    // User input mock
    mockInput.enqueueKey('G');
    mockInput.enqueueKey('m');
    mockInput.enqueueKey('a');
    mockInput.enqueueKey('i');
    mockInput.enqueueKey('l');
    mockInput.enqueueKey(KEY_OK); // Service name
    mockInput.enqueueKey('j');
    mockInput.enqueueKey('o');
    mockInput.enqueueKey('h');
    mockInput.enqueueKey('n');
    mockInput.enqueueKey(KEY_OK); // Username
    mockInput.enqueueKey('p');
    mockInput.enqueueKey('a');
    mockInput.enqueueKey('s');
    mockInput.enqueueKey('s');
    mockInput.enqueueKey(KEY_OK); // Password
    mockInput.enqueueKey('n');
    mockInput.enqueueKey('o');
    mockInput.enqueueKey('t');
    mockInput.enqueueKey('e');
    mockInput.enqueueKey(KEY_OK); // Notes

    bool result = controller.handleEntryCreation();

    TEST_ASSERT_TRUE(result);
    auto entries = entryService.getAllEntries();
    TEST_ASSERT_EQUAL(1, entries.size());
    TEST_ASSERT_EQUAL_STRING("Gmail", entries[0].getServiceName().c_str());
    TEST_ASSERT_EQUAL_STRING("john", entries[0].getUsername().c_str());
    TEST_ASSERT_EQUAL_STRING("pass", entries[0].getPassword().c_str());
    TEST_ASSERT_EQUAL_STRING("note", entries[0].getNotes().c_str());
}

void test_handleEntryUpdate() {
    MockView mockDisplay;
    MockInput mockInput;
    EntryRepository entryRepository;
    EntryService entryService(entryRepository);
    UsbService usbService;
    LedService ledService;
    NvsService nvsService;
    ModelTransformer modelTransformer;
    InactivityManager inactivityManager(mockDisplay);

    StringPromptSelector stringPromptSelector(mockDisplay, mockInput);
    VerticalSelector verticalSelector(mockDisplay, mockInput, inactivityManager);
    HorizontalSelector horizontalSelector(mockDisplay, mockInput, inactivityManager);
    FieldActionSelector fieldActionSelector(mockDisplay, mockInput, inactivityManager);
    ConfirmationSelector confirmationSelector(mockDisplay, mockInput);

    EntryController controller(mockDisplay, mockInput, horizontalSelector, verticalSelector,
                               fieldActionSelector, confirmationSelector, stringPromptSelector,
                               entryService, usbService, ledService, nvsService, modelTransformer);

    // Add Entry
    Entry entry("Gmail", "john", "pass", "note");
    entryService.addEntry(entry);

    // Input Mock
    mockInput.enqueueKey('y');
    mockInput.enqueueKey(KEY_OK); // updated username

    Field field("User", "john", "U");
    bool result = controller.handleEntryUpdate(entry, field);

    TEST_ASSERT_TRUE(result);
    auto entries = entryService.getAllEntries();
    TEST_ASSERT_EQUAL_STRING("johny", entries[0].getUsername().c_str());
}

void test_handleEntryDeletion() {
    MockView mockDisplay;
    MockInput mockInput;
    EntryRepository entryRepository;
    EntryService entryService(entryRepository);
    UsbService usbService;
    LedService ledService;
    NvsService nvsService;
    ModelTransformer modelTransformer;
    InactivityManager inactivityManager(mockDisplay);

    StringPromptSelector stringPromptSelector(mockDisplay, mockInput);
    VerticalSelector verticalSelector(mockDisplay, mockInput, inactivityManager);
    HorizontalSelector horizontalSelector(mockDisplay, mockInput, inactivityManager);
    FieldActionSelector fieldActionSelector(mockDisplay, mockInput, inactivityManager);
    ConfirmationSelector confirmationSelector(mockDisplay, mockInput);

    EntryController controller(mockDisplay, mockInput, horizontalSelector, verticalSelector,
                               fieldActionSelector, confirmationSelector, stringPromptSelector,
                               entryService, usbService, ledService, nvsService, modelTransformer);

    // Add 2 entries
    entryService.addEntry(Entry("Gmail", "john", "pass", "note"));
    entryService.addEntry(Entry("Yahoo", "doe", "word", "note2"));

    // Input mock
    mockInput.enqueueKey(KEY_OK); // Select first entry
    mockInput.enqueueKey(KEY_OK); // Confirm deletion
    mockInput.enqueueKey(KEY_ESC_CUSTOM);

    bool result = controller.handleEntryDeletion();

    TEST_ASSERT_TRUE(result);
    auto entries = entryService.getAllEntries();
    TEST_ASSERT_EQUAL(1, entries.size());
    TEST_ASSERT_EQUAL_STRING("Yahoo", entries[0].getServiceName().c_str());
}

#endif // TEST_ENTRY_CONTROLLER
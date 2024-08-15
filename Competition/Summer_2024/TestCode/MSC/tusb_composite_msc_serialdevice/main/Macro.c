
#include "Macro.h"

void OpenCMD()
{
    WriteCombinationOfCharacter(HID_KEY_GUI_LEFT);
    WriteEndCombination();
    vTaskDelay(pdMS_TO_TICKS(300));

    WriteASCII('c');
    WriteASCII('m');
    WriteASCII('d');
    vTaskDelay(pdMS_TO_TICKS(300));
    WriteCharacter(HID_KEY_ENTER);
    vTaskDelay(pdMS_TO_TICKS(300));
}

void OpenNotePad()
{
    WriteCombinationOfCharacter(HID_KEY_GUI_LEFT);
    WriteCombinationOfCharacterASCII('r');
    vTaskDelay(pdMS_TO_TICKS(100));
    WriteEndCombination();

    uint8_t notePad[9] = {'n','o','t','e','p','a','d','\0'};
    WriteWordsASCII(&notePad, 7);
    vTaskDelay(pdMS_TO_TICKS(100));
    WriteCharacter(HID_KEY_ENTER);
}

void WriteThis()
{
    char *str = "Hello, World!\nThis Frank speaking to you. I want to see how fast I can type this, do not judge. It is 11h34 at night. Maybe I will go to sleep after this, cause i have work tomorrow.\n";
    uint8_t *uint8Ptr = (uint8_t *)str;
    WriteWordsASCII(uint8Ptr, 183);
}

void GetRickRolled()
{
    OpenCMD();
    vTaskDelay(pdMS_TO_TICKS(500));
    char *str = "explorer \"https://www.youtube.com/watch?v=dQw4w9WgXcQ\"\n";
    uint8_t *uint8Ptr = (uint8_t *)str;
    WriteWordsASCII(uint8Ptr, 56);
}

void MatrixAttack()
{
    
}
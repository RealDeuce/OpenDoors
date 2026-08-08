# `od_draw_box()`

Draws a box on the screen in ANSI, AVATAR or RIP graphics modes.

## Synopsis

```c
BOOL od_draw_box(BYTE btLeft, BYTE btTop, BYTE btRight, BYTE btBottom);
```

## Return value

TRUE on success, FALSE on failure

## Description

This function is for use in ANSI, AVATAR or RIP graphics modes. This function will draw a box in the current display attribute, at the specified location on the screen. The boarder of the box is made up of the characters specified in the od_control. od_box_chars[] array. If AVATAR graphics mode is available, this function uses AVATAR control codes to display the box in less than 1/10 the length of time required to display the box in ANSI mode.

The first two parameters of this function, btLeft and btTop, specify the coordinates of the top, left-hand corner of the box to be draw. The third and fourth parameters, btRight and btBottom, specify the coordinates of the bottom, left-hand corner of the box. Like the values passed to the [`od_set_cursor()`](od_set_cursor.md) function, these coordinates are relative to the upper left-hand corner of the screen, with the position (1,1) being this corner.

As mentioned above, this function will display the window in the current text color. Thus, before calling this function, you should use either the [`od_set_color()`](od_set_color.md) or the [`od_set_attrib()`](od_set_attrib.md) function to specify the color in which you would like to have the window displayed.

Normally, the boarder of the window will be displayed using the IBM extended ASCII characters which produce a single line boarder. However, you may wish to have the boarder displayed using different characters. In this case, the characters used to display the boarder can be specified by the od_control. od_box_chars variable, described in the OpenDoors control structure section of this manual.

## Examples

As an example of the use of the [`od_draw_box()`](od_draw_box.md) function in conjunction with the [`od_edit_str()`](od_edit_str.md) function, we show a portion of a program which displays a window, and allows the user to input the name of a file they would like to upload, a description of the file, and whether they want it to be a private upload. The user is able to move among fields using the tab key, and select a "continue" button when they are finished. The function returns TRUE if the user selects continue, and FALSE if the user presses [ESCape].

```c
                                   // Main "dialog box" function
int get_information(char *filename, char *description,
                     char *private)
{
   char current_field=1;             // Currently selected field
   int choice;                                  // User's choice
```

od_set_color(L_WHITE,D_BLUE);               // Display window od_draw_box(10,5,70,13);

od_set_cursor(5,25);                  // Display window title od_set_color(L_GREEN,D_BLUE); od_disp_str(" ENTER FILENAME INFORMATION ");

od_set_color(L_CYAN,D_BLUE);     // Display fields and titles od_set_cursor(6,15); od_disp_str("FILENAME : "); od_repeat(176,13); od_set_cursor(7,12); od_disp_str("DESCRIPTION : "); od_repeat(176,43); od_set_cursor(8,16); od_disp_str("PRIVATE : "); od_repeat(176,2); draw_button();

filename[0]='\0';    // Blank out contents of input variables description[0]='\0'; private[0]='\0';

```c
for(;;)                               // Main dialog box loop
{
   if(current_field==4)             // If field is the button
   {
      od_set_color(L_GREEN,D_BLUE);       // Highlight button
      draw_button();
```

```c
do  // Loop until user presses [TAB], [ENTER], or [ESC]
{
   choice=od_get_key(TRUE);
} while(choice!=9 && choice!=13 && choice!=27);
```

```c
od_set_color(L_CYAN,D_BLUE);     // Un-highlight button
draw_button();
```

```c
   if(choice==13) return(TRUE);  // If [ENTER] was pressed
   if(choice==27) return(FALSE);   // If [ESC] was pressed
   current_field=1;        // Otherwise, [TAB] was pressed
}
```

```c
switch(current_field)        // According to selected field
 {                       // Input from the appropriate line
    case 1:
       choice=od_edit_str(filename,"FFFFFFFFFFFF",6,26,
                          0x1b,0x1a,176,
                          EDIT_FLAG_EDIT_STRING|
                          EDIT_FLAG_ALLOW_CANCEL|
                          EDIT_FLAG_FIELD_MODE|
                          EDIT_FLAG_KEEP_BLANK);
       break;
    case 2:
       choice=od_edit_str(description,
                          "*******************",
                          7,26,0x1b,0x1a,176,
                          EDIT_FLAG_EDIT_STRING|
                          EDIT_FLAG_ALLOW_CANCEL|
                          EDIT_FLAG_FIELD_MODE|
                          EDIT_FLAG_KEEP_BLANK);
```

```c
            break;
         case 3:
            choice=od_edit_str(private,"Y",8,26,
                               0x1b,0x1a,176,
                               EDIT_FLAG_EDIT_STRING|
                               EDIT_FLAG_ALLOW_CANCEL|
                               EDIT_FLAG_FIELD_MODE);
      }
                                     // If user pressed [ESCape]
      if(choice==EDIT_RETURN_CANCEL) return(FALSE);
                       // If user choice to go to previous field
      if(choice==EDIT_RETURN_PREVIOUS)
      {
         if(current_field==1)               // If at first field
            current_field=4;                 // Go to last field
         else                           // If not at first field
            --current_field;             // Go to previous field
      }
      else                           // If user chose next field
         ++current_field;                    // Go to next field
   }
}
```

```c
void draw_button(void)         // Function to display the button
{
   od_draw_box(12,10,23,12);              // Draw box for button
   od_set_cursor(11,14);
   od_disp_str("Continue");            // Display text in button
}
```

## Additional details

Coordinates are one-based and inclusive. The current attribute is used for the
border and interior. ANSI or AVATAR support is required. A zero left or top
coordinate, a right coordinate above 80, or a bottom coordinate above 25 sets
[`ERR_PARAMETER`](../constants/errors.md), and lack of graphics support sets
[`ERR_NOGRAPHICS`](../constants/errors.md).

The function does not verify that left precedes right or top precedes bottom.
The caller must provide an ordered rectangle with enough room for the requested
border; otherwise unsigned size calculations can wrap and the result is not
defined by the interface.

The function returns true after the box has been displayed.

## See also

[`od_set_color()`](od_set_color.md), [`od_set_attrib()`](od_set_attrib.md), [`od_clr_scr()`](od_clr_scr.md), [`od_edit_str()`](od_edit_str.md), [`od_set_cursor()`](od_set_cursor.md)

[`od_window_create()`](od_window_create.md),
[`od_set_attrib()`](od_set_attrib.md)

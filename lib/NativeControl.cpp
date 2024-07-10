#include "NativeControl.h"
using namespace lib;

NativeControl::NativeControl(HWND hParent, WORD ctrlId)
	: Window{GetDlgItem(hParent, ctrlId)}
{
}

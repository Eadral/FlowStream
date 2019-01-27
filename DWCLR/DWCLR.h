// DWCLR.h

#pragma once
#include "dwcore.h"
#include "Stdafx.h"

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Collections;
using namespace msclr::interop;

#pragma comment(lib, "dwcore.lib")
#pragma managed
namespace DWCLR {

	public ref class DW
	{
	public:
		String^ Version();
		
	};
}

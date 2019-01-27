// This is the main DLL file.

#include "stdafx.h"
#include "DWCLR.h"

namespace DWCLR {
	String^ DW::Version() {
		return marshal_as<String^>(DWCore::DW::Version());
	}

	
}
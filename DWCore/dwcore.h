#pragma once
#include "stdafx.h"

#ifdef CPPDLL_EXPORTS
#define CPP_EXPORTS __declspec(dllexport)
#else
#define CPP_EXPORTS __declspec(dllimport)
#endif


namespace DWCore {
	class DW {
	public:
		static CPP_EXPORTS std::string Version();
		Image OpenImage(std::string imagePath);
	};
	
	class Image {

	};

}



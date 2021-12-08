/* Copyright Jukka Jylänki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file MathAll.h
	@author Jukka Jylänki
	@brief A conveniency file to include all files in the folder Math/. */

#pragma once

#pragma warning(push)

// Warning disabled ---
#pragma warning( disable : 4005 )
#pragma warning( disable : 4006 )
#pragma warning( disable : 4009 )
#pragma warning( disable : 4099 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4244 )
#pragma warning( disable : 4530 )
#pragma warning( disable : 4577 )
#pragma warning( disable : 4834 )
#pragma warning( disable : 4838 )
#pragma warning( disable : 4996 )

#include "../MathBuildConfig.h"
#include "MathNamespace.h"

#include "BitOps.h"
#include "FixedPoint.h"
#include "float2.h"
#include "float3.h"
#include "float3x3.h"
#include "float3x4.h"
#include "float4.h"
#include "float4x4.h"
#include "FloatCmp.h"
#include "MathConstants.h"
#include "MathFunc.h"
#include "../MathGeoLibFwd.h"
#include "MatrixProxy.h"
#include "Polynomial.h"
#include "Quat.h"
#include "Rect.h"
#include "SSEMath.h"
#include "TransformOps.h"
#include "Swap.h"

#pragma warning(pop)
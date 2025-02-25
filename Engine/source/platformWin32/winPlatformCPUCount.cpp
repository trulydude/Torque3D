// Original code is:
// Copyright (c) 2005 Intel Corporation
// All Rights Reserved
//
// CPUCount.cpp : Detects three forms of hardware multi-threading support across IA-32 platform
//					The three forms of HW multithreading are: Multi-processor, Multi-core, and
//					HyperThreading Technology.
//					This application enumerates all the logical processors enabled by OS and BIOS,
//					determine the HW topology of these enabled logical processors in the system
//					using information provided by CPUID instruction.
//					A multi-processing system can support any combination of the three forms of HW
//					multi-threading support. The relevant topology can be identified using a
//					three level decomposition of the "initial APIC ID" into
//					Package_id, core_id, and SMT_id. Such decomposition provides a three-level map of
//					the topology of hardware resources and
//					allow multi-threaded software to manage shared hardware resources in
//					the platform to reduce resource contention

//					Multicore detection algorithm for processor and cache topology requires
//					all leaf functions of CPUID instructions be available. System administrator
//					must ensure BIOS settings is not configured to restrict CPUID functionalities.
//-------------------------------------------------------------------------------------------------

#include "platform/platform.h"

#if defined( TORQUE_OS_WIN )

#include "platform/platformCPUCount.h"
#include "console/console.h"
#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <assert.h>

namespace CPUInfo {

   // based on http://msdn.microsoft.com/en-us/library/ms683194.aspx

   // Helper function to count set bits in the processor mask.
   DWORD CountSetBits( ULONG_PTR bitMask )
   {
      DWORD LSHIFT = sizeof( ULONG_PTR ) * 8 - 1;
      DWORD bitSetCount = 0;
      ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
      DWORD i;

      for( i = 0; i <= LSHIFT; ++i )
      {
         bitSetCount += ((bitMask & bitTest) ? 1 : 0);
         bitTest /= 2;
      }

      return bitSetCount;
   }

   EConfig CPUCount( U32& TotAvailLogical, U32& TotAvailCore )
   {
      TotAvailLogical = 0;
      TotAvailCore = 0;

      PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
      DWORD returnLength = 0;

      // get buffer length
      DWORD rc = GetLogicalProcessorInformation( buffer, &returnLength );
      buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( returnLength );

      rc = GetLogicalProcessorInformation( buffer, &returnLength );

      // if we fail, assume single threaded
      if( FALSE == rc )
      {
         free( buffer );
         Con::errorf("Unable to determine CPU Count, assuming 1 core");
         TotAvailCore = 1;
         TotAvailLogical = 1;
         return CONFIG_SingleCoreAndHTNotCapable;
      }

#pragma warning( push )
#pragma warning( disable: 6011 )
      PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;

      DWORD byteOffset = 0;
      while( byteOffset + sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) <= returnLength )
      {
         if (ptr->Relationship == RelationProcessorCore)
         {
            TotAvailCore++;
            TotAvailLogical += CountSetBits(ptr->ProcessorMask);
         }

         byteOffset += sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
         ptr++;
      }

      free( buffer );
#pragma warning( pop )

      EConfig StatusFlag = CONFIG_SingleCoreAndHTNotCapable;

      if( TotAvailCore == 1 && TotAvailLogical > TotAvailCore )
         StatusFlag = CONFIG_SingleCoreHTEnabled;
      else if( TotAvailCore > 1 && TotAvailLogical == TotAvailCore )
         StatusFlag = CONFIG_MultiCoreAndHTNotCapable;
      else if( TotAvailCore > 1 && TotAvailLogical > TotAvailCore )
         StatusFlag = CONFIG_MultiCoreAndHTEnabled;

      return StatusFlag;
   }

} // namespace CPUInfo
#endif

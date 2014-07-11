using System;
using System.Runtime.InteropServices;

namespace Arika
{
	public class ArikaRaw
	{
		public delegate void CreateMainWindowType();
		public delegate void UpdateType();

		public static Delegate CreateMainWindow;
		public static Delegate Update;
			
		public static void ResolveSymbols(ArikaLoader.DllLoadUtils dllUtils, IntPtr dllHandle)
		{
			IntPtr funcHandle;

			funcHandle = dllUtils.GetProcAddress(dllHandle, "ar_window_create_main");

			CreateMainWindow = (CreateMainWindowType) Marshal.GetDelegateForFunctionPointer(
                           funcHandle , typeof (CreateMainWindowType));

			funcHandle = dllUtils.GetProcAddress(dllHandle, "ar_update");

			Update = (UpdateType) Marshal.GetDelegateForFunctionPointer(
                           funcHandle , typeof (UpdateType));
		}
	}
}

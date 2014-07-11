using System;
using System.IO;
using System.Runtime.InteropServices;

namespace Arika
{
	public class ArikaLoader
	{
		public interface DllLoadUtils 
		{
			IntPtr LoadLibrary(string fileName);
			void FreeLibrary(IntPtr handle);
			IntPtr GetProcAddress(IntPtr dllHandle, string name);
		}

		/*
		internal delegate void MyCrossplatformBar(int a, int b);


            DllLoadUtils dllLoadUtils = IsLinux() ? (DllLoadUtils) new DllLoadUtilsLinux() 
                                                                                             : new DllLoadUtilsWindows();
            string libraryName;

            if (IsLinux()) {
                libraryName = IntPtr.Size == 8 ? "mylib64.so" : "mylib32.so";
            } else {
                libraryName = IntPtr.Size == 8 ? "mylib64.dll" : "mylib32.dll";
            }

            var dllHandle = dllLoadUtils.LoadLibrary(libraryName);
            var functionHandle = dllLoadUtils.GetProcAddress(dllHandle, "MyCrossplatformBar");

            var method = (MyCrossplatformBar) Marshal.GetDelegateForFunctionPointer(
                           functionHandle, typeof (MyCrossplatformBar));
            method(10, 15);
        }

		*/
		
		public static bool LoadLibrary(string name)
		{
		    DllLoadUtils dllLoadUtils = null;

		    switch (RunningPlatform())
		    {
		    	case PlatformID.MacOSX : dllLoadUtils = (DllLoadUtils) new DllLoadUtilsMac(); break;
		    	case PlatformID.Unix : dllLoadUtils = (DllLoadUtils) new DllLoadUtilsUnix(); break;
		    	default : dllLoadUtils = (DllLoadUtils) new DllLoadUtilsWindows(); break;
		    }
		    
			IntPtr dllHandle;

           	dllHandle = dllLoadUtils.LoadLibrary(name);
			
			if (dllHandle == IntPtr.Zero)
			{
				Console.WriteLine("Unable to load dll {0}", name);
				return false;
			}

			ArikaRaw.ResolveSymbols(dllLoadUtils, dllHandle);

            return true;
        }

		public static PlatformID RunningPlatform()
		{
			switch (Environment.OSVersion.Platform)
			{
				case PlatformID.Unix:
					// Well, there are chances MacOSX is reported as Unix instead of MacOSX.
					// Instead of platform check, we'll do a feature checks (Mac specific root folders)
					if (Directory.Exists("/Applications")
						& Directory.Exists("/System")
						& Directory.Exists("/Users")
						& Directory.Exists("/Volumes"))
						return PlatformID.MacOSX;
					else
						return PlatformID.Unix;

				case PlatformID.MacOSX:
					return PlatformID.MacOSX;

				default:
					return PlatformID.Win32NT;
			}
		}

		
		// Windows version

		private class DllLoadUtilsWindows : DllLoadUtils 
		{
			void DllLoadUtils.FreeLibrary(IntPtr handle) 
			{ 
				FreeLibrary(handle);
			}

			IntPtr DllLoadUtils.GetProcAddress(IntPtr dllHandle, string name) 
			{
				return GetProcAddress(dllHandle, name);
			}

			IntPtr DllLoadUtils.LoadLibrary(string fileName) 
			{
				return LoadLibrary(fileName);
			}

			[DllImport("kernel32")]
			private static extern IntPtr LoadLibrary(string fileName);

			[DllImport("kernel32.dll")]
			private static extern int FreeLibrary(IntPtr handle);

			[DllImport("kernel32.dll")]
			private static extern IntPtr GetProcAddress(IntPtr handle, string procedureName);
    	}

    	// *Nix version

    	private class DllLoadUtilsUnix : DllLoadUtils 
    	{
        	public IntPtr LoadLibrary(string fileName) 
        	{
            	return dlopen(fileName, RTLD_NOW);
           	}

			public void FreeLibrary(IntPtr handle) 
			{
				dlclose(handle);
			}

        	public IntPtr GetProcAddress(IntPtr dllHandle, string name) 
        	{
				// clear previous errors if any
				dlerror();
				var res = dlsym(dllHandle, name);
				var errPtr = dlerror();

				if (errPtr != IntPtr.Zero) 
					throw new Exception("dlsym: " + Marshal.PtrToStringAnsi(errPtr));
            	
            	return res;
        	}

			const int RTLD_NOW = 2;

			[DllImport("libdl.so")]
			private static extern IntPtr dlopen(String fileName, int flags);
			
			[DllImport("libdl.so")]
			private static extern IntPtr dlsym(IntPtr handle, String symbol);

			[DllImport("libdl.so")]
			private static extern int dlclose(IntPtr handle);

			[DllImport("libdl.so")]
			private static extern IntPtr dlerror();
    	}

    	// Mac version

    	private class DllLoadUtilsMac : DllLoadUtils 
    	{
        	public IntPtr LoadLibrary(string fileName) 
        	{
            	return dlopen(fileName, RTLD_NOW);
           	}

			public void FreeLibrary(IntPtr handle) 
			{
				dlclose(handle);
			}

        	public IntPtr GetProcAddress(IntPtr dllHandle, string name) 
        	{
				// clear previous errors if any
				dlerror();
				var res = dlsym(dllHandle, name);
				var errPtr = dlerror();

				if (errPtr != IntPtr.Zero) 
					throw new Exception("dlsym: " + Marshal.PtrToStringAnsi(errPtr));
            	
            	return res;
        	}

			const int RTLD_NOW = 2;

			[DllImport("libc.dylib")]
			private static extern IntPtr dlopen(String fileName, int flags);
			
			[DllImport("libc.dylib")]
			private static extern IntPtr dlsym(IntPtr handle, String symbol);

			[DllImport("libc.dylib")]
			private static extern int dlclose(IntPtr handle);

			[DllImport("libc.dylib")]
			private static extern IntPtr dlerror();
    	}
	}
}

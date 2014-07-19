using System;
using Arika;

namespace CSharpTest
{
	class MainClass
	{
		public static void Main(string[] args)
		{
			string libraryName = "t2-output/macosx-clang-debug-default/libarika-qt.dylib";


			if (!ArikaLoader.LoadLibrary(libraryName))
			{
				Console.WriteLine("Unable to load dll: {0}", libraryName);
				return;
			}

			ArikaRaw.WindowCreateMain();

			for (;;)
			{
				if (ArikaRaw.Update() == 0)
					break;
			}


			Console.WriteLine("Hello World!");
		}
	}
}


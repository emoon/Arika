using System;
using Arika;

namespace CSharpTest
{
	class MainClass
	{
		public static void Main(string[] args)
		{
			string libraryName = "test";

			if (!ArikaLoader.LoadLibrary(libraryName))
			{
				Console.WriteLine("Unable to load dll: {0}", libraryName);
				return;
			}

			Console.WriteLine("Hello World!");
		}
	}
}


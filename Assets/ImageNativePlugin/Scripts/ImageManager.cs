using System;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;

public class ImageManager : MonoBehaviour
{
    [DllImport("UnityImageNativePlugin")]
	private static extern void PluginLoadImage(IntPtr path);

	[DllImport("UnityImageNativePlugin")]
	private static extern int PluginGetFrameCount();

	[DllImport("UnityImageNativePlugin")]
	private static extern int PluginGetPixel(int frame, int x, int y);

    public static void LoadImage(string path)
    {
        byte[] array = UTF8Encoding.UTF8.GetBytes(path);
        IntPtr lpData = Marshal.AllocHGlobal(array.Length);
        Marshal.Copy(array, 0, lpData, array.Length);
        PluginLoadImage(lpData);
        Marshal.FreeHGlobal(lpData);
    }

    static ImageManager()
    {
        LoadImage("Hello World!");

        int frameCount = PluginGetFrameCount();

        int color = PluginGetPixel(1, 2, 3);
    }
}

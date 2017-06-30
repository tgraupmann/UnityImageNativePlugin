using System;
using System.Runtime.InteropServices;
using UnityEngine;

public class ImageManager : MonoBehaviour
{
	[DllImport("UnityImageNativePlugin")]
	private static extern void LoadImage();

	[DllImport("UnityImageNativePlugin")]
	private static extern int GetFrameCount();

	[DllImport("UnityImageNativePlugin")]
	private static extern int GetPixel(int frame, int x, int y);
}

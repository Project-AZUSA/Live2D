using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace PNG
{
    class APIDraw
    {
        //The usual SINGLE-COLOR TransKey method for making the background transparent has failed due to random white pixels which are not removed
        //because their colors are slightly off the TransKey.
        //So an alternate way of drawing on form is utilised here using API calls

        #region CUSTOM PAINT METHODS ----------------------------------------------

        //Updates the Form's display using API calls
        static public void UpdateFormDisplay(Image backgroundImage, Form canvas)
        {
            Point pos = new Point(Screen.GetBounds(canvas).Width - backgroundImage.Size.Width,
                Screen.GetBounds(canvas).Height - backgroundImage.Size.Height);
            canvas.Location = pos;

            IntPtr screenDc = API.GetDC(IntPtr.Zero);
            IntPtr memDc = API.CreateCompatibleDC(screenDc);
            IntPtr hBitmap = IntPtr.Zero;
            IntPtr oldBitmap = IntPtr.Zero;

            try
            {
                //Display-image
                using (Bitmap bmp = new Bitmap(backgroundImage))
                {
                    hBitmap = bmp.GetHbitmap(Color.FromArgb(0));  //Set the fact that background is transparent
                    oldBitmap = API.SelectObject(memDc, hBitmap);

                    //Display-rectangle
                    Size size = bmp.Size;
                    Point pointSource = new Point(0, 0);
                    Point topPos = new Point(canvas.Left, canvas.Top);

                    //Set up blending options
                    API.BLENDFUNCTION blend = new API.BLENDFUNCTION();
                    blend.BlendOp = API.AC_SRC_OVER;
                    blend.BlendFlags = 0;
                    blend.SourceConstantAlpha = 255;
                    blend.AlphaFormat = API.AC_SRC_ALPHA;



                    //Draw image
                    API.UpdateLayeredWindow(canvas.Handle, screenDc, ref topPos, ref size, memDc, ref pointSource, 0, ref blend, API.ULW_ALPHA);

                    //Clean-up
                    bmp.Dispose();
                    API.ReleaseDC(IntPtr.Zero, screenDc);
                    if (hBitmap != IntPtr.Zero)
                    {
                        API.SelectObject(memDc, oldBitmap);
                        API.DeleteObject(hBitmap);
                    }
                    API.DeleteDC(memDc);
                }
            }
            catch (Exception)
            {
            }
        }
        #endregion

        #region API
        internal class API
        {
            public const byte AC_SRC_OVER = 0x00;
            public const byte AC_SRC_ALPHA = 0x01;
            public const Int32 ULW_ALPHA = 0x00000002;

            [StructLayout(LayoutKind.Sequential, Pack = 1)]
            public struct BLENDFUNCTION
            {
                public byte BlendOp;
                public byte BlendFlags;
                public byte SourceConstantAlpha;
                public byte AlphaFormat;
            }

            [DllImport("user32.dll", ExactSpelling = true, SetLastError = true)]
            public static extern bool UpdateLayeredWindow(IntPtr hwnd, IntPtr hdcDst, ref Point pptDst, ref Size psize, IntPtr hdcSrc, ref Point pprSrc, Int32 crKey, ref BLENDFUNCTION pblend, Int32 dwFlags);


            [DllImport("user32.dll", ExactSpelling = true, SetLastError = true)]
            public static extern IntPtr GetDC(IntPtr hWnd);

            [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
            public static extern IntPtr CreateCompatibleDC(IntPtr hDC);

            [DllImport("user32.dll", ExactSpelling = true)]
            public static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

            [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
            public static extern bool DeleteDC(IntPtr hdc);


            [DllImport("gdi32.dll", ExactSpelling = true)]
            public static extern IntPtr SelectObject(IntPtr hDC, IntPtr hObject);

            [DllImport("gdi32.dll", ExactSpelling = true, SetLastError = true)]
            public static extern bool DeleteObject(IntPtr hObject);
        }
        #endregion
    }
}

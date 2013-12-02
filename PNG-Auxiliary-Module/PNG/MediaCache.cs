using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace PNG
{
    class MediaCache
    {
        static public Dictionary<string, Image> preloadedImg = new Dictionary<string, Image>();
    }
}

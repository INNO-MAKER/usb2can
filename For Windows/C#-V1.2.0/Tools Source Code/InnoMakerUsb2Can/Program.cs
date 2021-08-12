using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace InnoMakerUsb2Can
{
    static class Program
    {
        /// <summary>
        /// Main
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new InnoMakerusb2Can());
        }
    }
}

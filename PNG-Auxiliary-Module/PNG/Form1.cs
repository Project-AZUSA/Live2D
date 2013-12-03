using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;

namespace PNG
{
    public partial class Form1 : Form
    {

        Thread listener;
        int AZUSAPid;
        bool EXITING = false;

        public List<string> currentAniFrames = new List<string>();
        public int currentFrame = 0;

        public Form1()
        {
            InitializeComponent();
            MakeDraggable();
        }

        //For API Graphics Calls
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                cp.ExStyle |= 0x00080000; // Required: set WS_EX_LAYERED extended style
                return cp;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Console.WriteLine("GetAzusaPid()");
            AZUSAPid = Convert.ToInt32(Console.ReadLine());

            Console.WriteLine("LinkRID(IMG,false)");
            Console.WriteLine("LinkRID(ANIMATE,false)");
            Console.WriteLine("LinkRID(SAY,false)");

            listener = new Thread(AZUSAListener);
            listener.Start();

            Console.WriteLine("RegisterAs(Output)");

            timer1.Start();

        }

        void AZUSAListener()
        {
            string msg = "";
            string[] parsed;
            string RID;
            string arg;
            while (!EXITING)
            {                
                msg = Console.ReadLine();

                if (msg != null)
                {
                    parsed = msg.Split('(');
                    RID = parsed[0];
                    arg = msg.Replace(RID + "(", "").Trim().TrimEnd(')');

                    if (RID == "ANIMATE")
                    {

                        ANIMATE(arg);
                    }
                    else if (RID == "IMG")
                    {
                        currentAniFrames.Clear();
                        currentFrame = 0;
                        if (File.Exists(Environment.CurrentDirectory + @"\res\img\" + arg))
                        {
                            currentAniFrames.Add(@"img\" + arg);
                        }
                    }
                    else if (RID == "SAY")
                    {
                        int id = 0;
                        if (Int32.TryParse(arg.Split(',')[0], out id))
                        {

                            SAY(id, arg.Replace(id + ",", ""));
                        }
                        else
                        {
                            SAY(id, arg);
                        }
                    }

                }
            }

        }

        private void ANIMATE(string arg)
        {

            currentAniFrames.Clear();
            currentFrame = 0;
            foreach (string path in Directory.GetFiles(Environment.CurrentDirectory + @"\res\ani\" + arg.Trim().Trim('\\') + @"\", "*.png", SearchOption.TopDirectoryOnly))
            {
                currentAniFrames.Add(path.Replace(Environment.CurrentDirectory + @"\res\", ""));
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                Process.GetProcessById(AZUSAPid);
            }
            catch
            {
                Application.Exit();
            }

            if (currentAniFrames.Count == 0)
            {
                this.Hide();
            }
            else
            {
                this.Show();
            }
            if (currentAniFrames.Count > currentFrame && this.Visible)
            {
                string frame;
                try
                {
                    frame = currentAniFrames[currentFrame];
                }
                catch
                {
                    return;
                }

                //put image
                if (!MediaCache.preloadedImg.ContainsKey(frame))
                {
                    if (File.Exists(Environment.CurrentDirectory + @"\res\" + frame))
                    {

                        MediaCache.preloadedImg.Add(frame, Image.FromFile(Environment.CurrentDirectory + @"\res\" + frame));

                    }

                }

                this.Size = MediaCache.preloadedImg[frame].Size;
                APIDraw.UpdateFormDisplay(MediaCache.preloadedImg[frame], this);


                //shift frame
                if (currentFrame != currentAniFrames.Count - 1)
                {
                    currentFrame = currentFrame + 1;
                }
                else
                {
                    currentFrame = 0;
                }
            }
        }




        string emotion = "";
        bool noAnimation = false;



        private void SAY(int id, string content)
        {
            string[] lines;
            #region decide emotion
            //get current emotion
            Console.WriteLine("EMOTION?");
            emotion = Console.ReadLine();

            //if emotion doesn't exist, set it to empty
            if (emotion == "EMOTION")
            {
                emotion = "";
            }

            //load emotion settings
            try
            {
                lines = System.IO.File.ReadAllLines(Environment.CurrentDirectory + @"\EMOTION.txt");
            }
            catch
            {
                Console.WriteLine(@"ERR(Unable to load the emotion settings. [PNGUI])");
                Console.Out.Flush();
                return;
            }

            //decide emotion
            int numline = 1;
            foreach (string line in lines)
            {
                if (line.Trim() != "" || !line.StartsWith("#")) //allows some formatting
                {
                    string[] separators = { "," };
                    string[] entry = line.Split(separators, StringSplitOptions.RemoveEmptyEntries);
                    try
                    {
                        if (emotion == entry[0])
                        {
                            File.WriteAllText(Environment.CurrentDirectory + @"\spchGen.bat", "cd %~dp0\n" + entry[1]);
                        }
                    }
                    catch
                    {
                        Console.WriteLine(@"ERR(Unable to parse line " + numline.ToString() + " of the emotion settings. [PNGUI])");
                    }
                }
                numline++;
            }


            #endregion

            int length;
            #region coordinate animation
            foreach (string part in content.Split(new char[] { '、', ' ', '\t' }))
            {
                if (part.Trim() != "")
                {

                    //Generate speech wav
                    length = GenSpeech(part);

                    if (length == 0) { return; }

                    //float[] amps = AmplitudesFromFile(Environment.CurrentDirectory + @"\res\sound\speech.wav");

                    if (!noAnimation)
                    {
                        Console.WriteLine(@"UI_PlaySound(res\sound\speech.wav," + id + ")");
                        System.Threading.Thread.Sleep(length);
                    }
                    else
                    {
                        using (System.Media.SoundPlayer player = new System.Media.SoundPlayer(Environment.CurrentDirectory + @"\res\sound\speech.wav"))
                        {
                            player.PlaySync();
                        }
                    }
                    Console.WriteLine("UI_SetMouthOpen(0," + id + ")");

                }

            }

            #endregion
        }


        [System.Runtime.InteropServices.DllImport("winmm.dll")]
        private static extern uint mciSendString(
            string command,
            StringBuilder returnValue,
            int returnLength,
            IntPtr winHandle);

        int GenSpeech(string text)
        {
            string txt;
            if (text.StartsWith("*"))
            {
                noAnimation = true;
                txt = text.TrimStart('*');
            }
            else
            {
                noAnimation = false;
                txt = text;
            }

            File.WriteAllText(Environment.CurrentDirectory + @"\text.txt", txt, Encoding.GetEncoding(932));

            Process JTalk = new Process();
            JTalk.StartInfo.CreateNoWindow = true;
            JTalk.StartInfo.UseShellExecute = false;
            JTalk.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            JTalk.StartInfo.FileName = "cmd.exe";
            JTalk.StartInfo.Arguments = "/c \"" + Environment.CurrentDirectory + "\\spchGen.bat\"";


            for (int i = 0; i < 5; i++)
            {
                JTalk.Start();

                JTalk.WaitForExit();
                if (File.Exists(Environment.CurrentDirectory + @"\res\sound\speech.wav"))
                {
                    StringBuilder lengthBuf = new StringBuilder(32);

                    mciSendString(string.Format("open \"{0}\" type waveaudio alias wave", Environment.CurrentDirectory + @"\res\sound\speech.wav"), null, 0, IntPtr.Zero);
                    mciSendString("status wave length", lengthBuf, lengthBuf.Capacity, IntPtr.Zero);
                    mciSendString("close wave", null, 0, IntPtr.Zero);

                    int length = 0;
                    int.TryParse(lengthBuf.ToString(), out length);

                    return length;
                }
            }

            return 0;
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            EXITING = true;
            listener.Abort();
            listener = null;
        }


        #region Draggable Form
        #region Declarations
        private bool drag = false;
        private Point start_point = new Point(0, 0);
        private bool draggable = true;
        private string exclude_list = "";
        #endregion

        #region Overriden Functions

        protected override void OnControlAdded(ControlEventArgs e)
        {
            //
            //Add Mouse Event Handlers for each control added into the form,
            //if Draggable property of the form is set to true and the control
            //name is not in the ExcludeList.Exclude list is the comma separated
            //list of the Controls for which you do not require the mouse handler 
            //to be added. For Example a button.  
            //
            if (this.Draggable && (this.ExcludeList.IndexOf(e.Control.Name) == -1))
            {
                e.Control.MouseDown += new MouseEventHandler(Form_MouseDown);
                e.Control.MouseUp += new MouseEventHandler(Form_MouseUp);
                e.Control.MouseMove += new MouseEventHandler(Form_MouseMove);
            }
            base.OnControlAdded(e);
        }

        #endregion

        #region Event Handlers

        void Form_MouseDown(object sender, MouseEventArgs e)
        {
            //
            //On Mouse Down set the flag drag=true and 
            //Store the clicked point to the start_point variable
            //
            this.drag = true;
            this.start_point = new Point(e.X, e.Y);
        }

        void Form_MouseUp(object sender, MouseEventArgs e)
        {
            //
            //Set the drag flag = false;
            //
            this.drag = false;
        }

        void Form_MouseMove(object sender, MouseEventArgs e)
        {
            //
            //If drag = true, drag the form
            //
            if (this.drag)
            {
                Point p1 = new Point(e.X, e.Y);
                Point p2 = this.PointToScreen(p1);
                Point p3 = new Point(p2.X - this.start_point.X,
                                     p2.Y - this.start_point.Y);
                this.Location = p3;
            }
        }

        #endregion

        #region Properties

        public string ExcludeList
        {
            set
            {
                this.exclude_list = value;
            }
            get
            {
                return this.exclude_list.Trim();
            }
        }

        public bool Draggable
        {
            set
            {
                this.draggable = value;
            }
            get
            {
                return this.draggable;
            }
        }

        #endregion

        void MakeDraggable()
        {
            this.MouseDown += new MouseEventHandler(Form_MouseDown);
            this.MouseUp += new MouseEventHandler(Form_MouseUp);
            this.MouseMove += new MouseEventHandler(Form_MouseMove);
        }
        #endregion

    }
}

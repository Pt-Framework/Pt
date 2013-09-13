/*
 * Copyright (C) 2013 Atesion GmbH
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA*/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace Atesion.Tools.BinToHeader
{
    public partial class MainFrame : Form
    {
        private int _progress = 0;
        private Thread _thread;
        private bool _running;
        private Exception _errorStatus;
        private delegate void OnEndDelegate();
        private System.Windows.Forms.Timer _timer = new System.Windows.Forms.Timer();

        public MainFrame()
        {
            InitializeComponent();
        }

        private void OnGetBinFileClick(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();

            dlg.Filter = "*.*|*.*";

            if(dlg.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            resFile.Text = dlg.FileName;
        }

        private void OnGetHeaderFileClick(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "*.h|*.h|*.*|*.*";

            if(dlg.ShowDialog() != System.Windows.Forms.DialogResult.OK)
                return;

            headerFile.Text = dlg.FileName;
        }


        private static void RunStatic(object me)
        {
            MainFrame frame = (MainFrame) me;
            frame.run();
        }

        private void run()
        {
            _errorStatus = null;
            int modulo = Convert.ToInt32(newLineAfter.Value);

            try
            {
                using (StreamWriter file = new System.IO.StreamWriter(headerFile.Text))
                {
                    if(printHeader.Checked)
                        file.WriteLine("//Generated with Atesion GmbH, BinToHeader tool. Generation time: " + DateTime.Now.ToString());

                    file.WriteLine("#ifndef " + incGuard.Text);
                    file.WriteLine("#define " + incGuard.Text);
                    file.WriteLine();

                    string[] array = null;

                    if(nSpace.Text != "")
                    {
                        array =  nSpace.Text.Split(':');

                        foreach(string ns in array)
                        {
                            if( ns != "")
                                file.WriteLine("namespace " + ns+"{");
                        }
                    }

                    file.WriteLine();
                    using (FileStream fs = new FileStream(resFile.Text, FileMode.Open, FileAccess.Read))
                    {
                        file.WriteLine("const unsigned int " + varName.Text +"Size = " + fs.Length.ToString() + ";");
                        file.WriteLine();
                        file.WriteLine("const unsigned char " + varName.Text +"[] = {");

                        BinaryReader br = new BinaryReader(fs);

                        for (int i = 0; (i < fs.Length) && _running; ++i)
                        {

                            if( i != 0)
                                file.Write(", ");

                            if (i % modulo == 0 && i != 0)
                                file.Write("\n");
                            
                            byte b = br.ReadByte();

                            file.Write("0x" + b.ToString("X2"));
                            _progress = 100*i/ (int)fs.Length;
                        }

                        file.WriteLine("};");
                        file.WriteLine();
                        
                        if(nSpace.Text != "")
                        {                    
                            foreach(string ns in array)
                            {
                                if( ns != "")
                                    file.Write("}");
                            }

                            file.WriteLine();
                        }

                        file.WriteLine();
                        file.WriteLine("#endif");
                        file.WriteLine();
                        file.Close();
                   }                    
                }
            }
            catch(Exception ex)
            {
                _errorStatus = ex;
            }

            
            if(_running)
            {
                _running = false;
                this.Invoke(new OnEndDelegate(OnEnd));
            }
        }


        private void OnEnd()
        {
            Stop();

            if(_errorStatus != null)
            {
                MessageBox.Show(_errorStatus.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                progress.Value = 100;
                persent.Text = "100 (%)";
            }

            _errorStatus = null;
        }

        private void OnGenerateClick(object sender, EventArgs e)
        {
            _running = true;
            generate.Enabled = false;
            cancel.Enabled = true;
            OK.Enabled = false;
            _timer.Interval = 500;
            _timer.Tick += new EventHandler(OnTimerTick);
            _timer.Start();

            _thread = new Thread(new ParameterizedThreadStart(RunStatic));
            _thread.Start(this);                        
        }

        private void OnTimerTick(object sender, EventArgs e)
        {            
            progress.Value = _progress;
            persent.Text = _progress.ToString() + " (%)";
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = _running;
            base.OnClosing(e);
        }

        private void OnCancelClick(object sender, EventArgs e)
        {
            _running = false;
            _thread.Join();
            _thread = null;

            Stop();
        }

        private void Stop()
        {
            _timer.Stop();
            generate.Enabled = true;
            cancel.Enabled = false;
            OK.Enabled = true;
        }

        private void OnOKClick(object sender, EventArgs e)
        {
            Close();
        }
    }
}

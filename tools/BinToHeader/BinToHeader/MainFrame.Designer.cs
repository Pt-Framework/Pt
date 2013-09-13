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
namespace Atesion.Tools.BinToHeader
{
    partial class MainFrame
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainFrame));
            this.OK = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.printHeader = new System.Windows.Forms.CheckBox();
            this.label9 = new System.Windows.Forms.Label();
            this.newLineAfter = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.nSpace = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.varName = new System.Windows.Forms.TextBox();
            this.incGuard = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.getHeaderFile = new System.Windows.Forms.Button();
            this.headerFile = new System.Windows.Forms.TextBox();
            this.resFile = new System.Windows.Forms.TextBox();
            this.getBinFile = new System.Windows.Forms.Button();
            this.cancel = new System.Windows.Forms.Button();
            this.persent = new System.Windows.Forms.Label();
            this.progress = new System.Windows.Forms.ProgressBar();
            this.label6 = new System.Windows.Forms.Label();
            this.generate = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.newLineAfter)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // OK
            // 
            this.OK.Location = new System.Drawing.Point(406, 316);
            this.OK.Name = "OK";
            this.OK.Size = new System.Drawing.Size(75, 23);
            this.OK.TabIndex = 0;
            this.OK.Text = "Close";
            this.OK.UseVisualStyleBackColor = true;
            this.OK.Click += new System.EventHandler(this.OnOKClick);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.printHeader);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.newLineAfter);
            this.groupBox1.Controls.Add(this.label8);
            this.groupBox1.Controls.Add(this.label5);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.nSpace);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.varName);
            this.groupBox1.Controls.Add(this.incGuard);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.getHeaderFile);
            this.groupBox1.Controls.Add(this.headerFile);
            this.groupBox1.Controls.Add(this.resFile);
            this.groupBox1.Controls.Add(this.getBinFile);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(469, 194);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Parameter:";
            // 
            // printHeader
            // 
            this.printHeader.AutoSize = true;
            this.printHeader.Checked = true;
            this.printHeader.CheckState = System.Windows.Forms.CheckState.Checked;
            this.printHeader.Location = new System.Drawing.Point(281, 163);
            this.printHeader.Name = "printHeader";
            this.printHeader.Size = new System.Drawing.Size(133, 17);
            this.printHeader.TabIndex = 15;
            this.printHeader.Text = "Output generation time";
            this.printHeader.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(187, 164);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(34, 13);
            this.label9.TabIndex = 14;
            this.label9.Text = "(Byte)";
            // 
            // newLineAfter
            // 
            this.newLineAfter.Location = new System.Drawing.Point(126, 162);
            this.newLineAfter.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.newLineAfter.Name = "newLineAfter";
            this.newLineAfter.Size = new System.Drawing.Size(55, 20);
            this.newLineAfter.TabIndex = 13;
            this.newLineAfter.Value = new decimal(new int[] {
            20,
            0,
            0,
            0});
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(13, 164);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(75, 13);
            this.label8.TabIndex = 12;
            this.label8.Text = "New line after:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 56);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(61, 13);
            this.label5.TabIndex = 11;
            this.label5.Text = "Header file:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(13, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(72, 13);
            this.label4.TabIndex = 10;
            this.label4.Text = "Resource file:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 132);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Namespace:";
            // 
            // nSpace
            // 
            this.nSpace.Location = new System.Drawing.Point(126, 129);
            this.nSpace.Name = "nSpace";
            this.nSpace.Size = new System.Drawing.Size(288, 20);
            this.nSpace.TabIndex = 8;
            this.nSpace.Text = "company::project::filename";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 106);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(97, 13);
            this.label2.TabIndex = 7;
            this.label2.Text = "Variable res. name:";
            // 
            // varName
            // 
            this.varName.Location = new System.Drawing.Point(126, 103);
            this.varName.Name = "varName";
            this.varName.Size = new System.Drawing.Size(288, 20);
            this.varName.TabIndex = 6;
            this.varName.Text = "g_myResource";
            // 
            // incGuard
            // 
            this.incGuard.Location = new System.Drawing.Point(126, 77);
            this.incGuard.Name = "incGuard";
            this.incGuard.Size = new System.Drawing.Size(288, 20);
            this.incGuard.TabIndex = 5;
            this.incGuard.Text = "NAMESPACE_FILENAME_H";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 80);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(82, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Inclusion guard:";
            // 
            // getHeaderFile
            // 
            this.getHeaderFile.Location = new System.Drawing.Point(413, 49);
            this.getHeaderFile.Name = "getHeaderFile";
            this.getHeaderFile.Size = new System.Drawing.Size(50, 23);
            this.getHeaderFile.TabIndex = 3;
            this.getHeaderFile.Text = "...";
            this.getHeaderFile.UseVisualStyleBackColor = true;
            this.getHeaderFile.Click += new System.EventHandler(this.OnGetHeaderFileClick);
            // 
            // headerFile
            // 
            this.headerFile.Location = new System.Drawing.Point(126, 50);
            this.headerFile.Name = "headerFile";
            this.headerFile.Size = new System.Drawing.Size(288, 20);
            this.headerFile.TabIndex = 2;
            // 
            // resFile
            // 
            this.resFile.Location = new System.Drawing.Point(126, 22);
            this.resFile.Name = "resFile";
            this.resFile.ReadOnly = true;
            this.resFile.Size = new System.Drawing.Size(288, 20);
            this.resFile.TabIndex = 1;
            // 
            // getBinFile
            // 
            this.getBinFile.Location = new System.Drawing.Point(413, 20);
            this.getBinFile.Name = "getBinFile";
            this.getBinFile.Size = new System.Drawing.Size(50, 23);
            this.getBinFile.TabIndex = 0;
            this.getBinFile.Text = "...";
            this.getBinFile.UseVisualStyleBackColor = true;
            this.getBinFile.Click += new System.EventHandler(this.OnGetBinFileClick);
            // 
            // cancel
            // 
            this.cancel.Enabled = false;
            this.cancel.Location = new System.Drawing.Point(338, 58);
            this.cancel.Name = "cancel";
            this.cancel.Size = new System.Drawing.Size(75, 23);
            this.cancel.TabIndex = 16;
            this.cancel.Text = "Cancel";
            this.cancel.UseVisualStyleBackColor = true;
            this.cancel.Click += new System.EventHandler(this.OnCancelClick);
            // 
            // persent
            // 
            this.persent.AutoSize = true;
            this.persent.Location = new System.Drawing.Point(420, 23);
            this.persent.Name = "persent";
            this.persent.Size = new System.Drawing.Size(24, 13);
            this.persent.TabIndex = 15;
            this.persent.Text = "0 %";
            // 
            // progress
            // 
            this.progress.Location = new System.Drawing.Point(69, 19);
            this.progress.Name = "progress";
            this.progress.Size = new System.Drawing.Size(344, 23);
            this.progress.TabIndex = 14;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 24);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(51, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "Progress:";
            // 
            // generate
            // 
            this.generate.Location = new System.Drawing.Point(257, 58);
            this.generate.Name = "generate";
            this.generate.Size = new System.Drawing.Size(75, 23);
            this.generate.TabIndex = 12;
            this.generate.Text = "Generate";
            this.generate.UseVisualStyleBackColor = true;
            this.generate.Click += new System.EventHandler(this.OnGenerateClick);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.progress);
            this.groupBox2.Controls.Add(this.cancel);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.persent);
            this.groupBox2.Controls.Add(this.generate);
            this.groupBox2.Location = new System.Drawing.Point(13, 212);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(468, 98);
            this.groupBox2.TabIndex = 17;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Generate:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 321);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(276, 13);
            this.label7.TabIndex = 18;
            this.label7.Text = "This is a free software! You can do what you want with it!";
            // 
            // MainFrame
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(493, 352);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.OK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainFrame";
            this.Text = "Binary Resource to C++ Header Generator";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.newLineAfter)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button OK;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label persent;
        private System.Windows.Forms.ProgressBar progress;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button generate;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox nSpace;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox varName;
        private System.Windows.Forms.TextBox incGuard;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button getHeaderFile;
        private System.Windows.Forms.TextBox headerFile;
        private System.Windows.Forms.TextBox resFile;
        private System.Windows.Forms.Button getBinFile;
        private System.Windows.Forms.Button cancel;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown newLineAfter;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox printHeader;
    }
}


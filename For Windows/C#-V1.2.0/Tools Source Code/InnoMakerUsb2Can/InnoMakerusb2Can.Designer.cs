using System.ComponentModel;
using System.Drawing;
using System.Drawing.Design;
using System.Windows.Forms;

namespace InnoMakerUsb2Can
{

    class ListViewEx : System.Windows.Forms.ListView

    {

        public ListViewEx()

        {

            // 开启双缓冲

            this.SetStyle(System.Windows.Forms.ControlStyles.OptimizedDoubleBuffer | System.Windows.Forms.ControlStyles.AllPaintingInWmPaint, true);



            // Enable the OnNotifyMessage event so we get a chance to filter out 

            // Windows messages before they get to the form's WndProc

            this.SetStyle(System.Windows.Forms.ControlStyles.EnableNotifyMessage, true);

        }



        protected override void OnNotifyMessage(System.Windows.Forms.Message m)

        {

            //Filter out the WM_ERASEBKGND message

            if (m.Msg != 0x14)

            {

                base.OnNotifyMessage(m);

            }

        }

    }

    //与SendMessage EM_SETCUEBANNER消息相比，它能改变字体绘制颜色，EM_SETCUEBANNER只限定了DimGray颜色，太深
    [ToolboxBitmap(typeof(TextBox))]
    public class TextBoxEx : TextBox
    {
        private const int WM_PAINT = 0xF;
        private Color placeHolderColor = Color.Gray;
        private string placeHolderText;

        [DefaultValue("")]
        [Localizable(true)]
        [Editor("System.ComponentModel.Design.MultilineStringEditor, System.Design, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a", typeof(UITypeEditor))]
        public string PlaceHolderText
        {
            get { return this.placeHolderText; }
            set
            {
                this.placeHolderText = value;
                Invalidate();
            }
        }

        protected override void WndProc(ref Message m)
        {
            base.WndProc(ref m);
            if (m.Msg == WM_PAINT)
                WmPaint(ref m);
        }

        private void WmPaint(ref Message m)
        {
            if (!string.IsNullOrEmpty(this.Text) || string.IsNullOrEmpty(this.placeHolderText) || Focused)
                return;

            using (var g = Graphics.FromHwnd(this.Handle))
            {
                var flag = TextFormatFlags.EndEllipsis;
                //它反着来
                if (RightToLeft == RightToLeft.Yes)
                {
                    flag |= TextFormatFlags.RightToLeft;
                    if (TextAlign == HorizontalAlignment.Left)
                        flag |= TextFormatFlags.Right;
                    else if (TextAlign == HorizontalAlignment.Right)
                        flag |= TextFormatFlags.Left;
                }
                else
                {
                    switch (TextAlign)
                    {
                        case HorizontalAlignment.Center:
                            flag |= TextFormatFlags.HorizontalCenter;
                            break;
                        case HorizontalAlignment.Right:
                            flag |= TextFormatFlags.Right;
                            break;
                        default:
                            break;
                    }
                }

                var r = this.ClientRectangle;
                r.Offset(-1, 1);
                TextRenderer.DrawText(
                    g,
                    this.placeHolderText,
                    Font,
                    r,
                    this.placeHolderColor,
                    flag
                );
            }
        }
    }
    partial class InnoMakerusb2Can
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InnoMakerusb2Can));
            this.label1 = new System.Windows.Forms.Label();
            this.DeviceComboBox = new System.Windows.Forms.ComboBox();
            this.ScanDeviceBtn = new System.Windows.Forms.Button();
            this.OpenDeviceBtn = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.FormatComboBox = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.TypeComboBox = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.ModeComboBox = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.BaudRateComboBox = new System.Windows.Forms.ComboBox();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.NumberSendTextBox = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.SendIntervalTextBox = new System.Windows.Forms.TextBox();
            this.IDAutoIncCheckBox = new System.Windows.Forms.CheckBox();
            this.DataAutoIncCheckBox = new System.Windows.Forms.CheckBox();
            this.HideErrorFrameCheckBox = new System.Windows.Forms.CheckBox();
            this.DelayedSendBtn = new System.Windows.Forms.Button();
            this.SendBtn = new System.Windows.Forms.Button();
            this.ClearBtn = new System.Windows.Forms.Button();
            this.ExportBtn = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.LangCombox = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.FrameIdTextBox = new InnoMakerUsb2Can.TextBoxEx();
            this.DataTextBox = new InnoMakerUsb2Can.TextBoxEx();
            this.listView = new ListViewEx();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader9 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader10 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // DeviceComboBox
            // 
            this.DeviceComboBox.FormattingEnabled = true;
            resources.ApplyResources(this.DeviceComboBox, "DeviceComboBox");
            this.DeviceComboBox.Name = "DeviceComboBox";
            this.DeviceComboBox.SelectedIndexChanged += new System.EventHandler(this.DeviceComboBox_SelectedIndexChanged);
            // 
            // ScanDeviceBtn
            // 
            resources.ApplyResources(this.ScanDeviceBtn, "ScanDeviceBtn");
            this.ScanDeviceBtn.Name = "ScanDeviceBtn";
            this.ScanDeviceBtn.UseVisualStyleBackColor = true;
            this.ScanDeviceBtn.Click += new System.EventHandler(this.ScanDeviceBtn_Click);
            // 
            // OpenDeviceBtn
            // 
            resources.ApplyResources(this.OpenDeviceBtn, "OpenDeviceBtn");
            this.OpenDeviceBtn.Name = "OpenDeviceBtn";
            this.OpenDeviceBtn.UseVisualStyleBackColor = true;
            this.OpenDeviceBtn.Click += new System.EventHandler(this.OpenDeviceBtn_Click);
            // 
            // label2
            // 
            resources.ApplyResources(this.label2, "label2");
            this.label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(this.label3, "label3");
            this.label3.Name = "label3";
            // 
            // FormatComboBox
            // 
            this.FormatComboBox.FormattingEnabled = true;
            resources.ApplyResources(this.FormatComboBox, "FormatComboBox");
            this.FormatComboBox.Name = "FormatComboBox";
            this.FormatComboBox.SelectedIndexChanged += new System.EventHandler(this.FormatComboBox_SelectedIndexChanged);
            // 
            // label4
            // 
            resources.ApplyResources(this.label4, "label4");
            this.label4.Name = "label4";
            // 
            // TypeComboBox
            // 
            this.TypeComboBox.FormattingEnabled = true;
            resources.ApplyResources(this.TypeComboBox, "TypeComboBox");
            this.TypeComboBox.Name = "TypeComboBox";
            this.TypeComboBox.SelectedIndexChanged += new System.EventHandler(this.TypeComboBox_SelectedIndexChanged);
            // 
            // label5
            // 
            resources.ApplyResources(this.label5, "label5");
            this.label5.Name = "label5";
            // 
            // ModeComboBox
            // 
            this.ModeComboBox.FormattingEnabled = true;
            resources.ApplyResources(this.ModeComboBox, "ModeComboBox");
            this.ModeComboBox.Name = "ModeComboBox";
            this.ModeComboBox.SelectedIndexChanged += new System.EventHandler(this.ModeComboBox_SelectedIndexChanged);
            // 
            // label6
            // 
            resources.ApplyResources(this.label6, "label6");
            this.label6.Name = "label6";
            // 
            // BaudRateComboBox
            // 
            this.BaudRateComboBox.FormattingEnabled = true;
            resources.ApplyResources(this.BaudRateComboBox, "BaudRateComboBox");
            this.BaudRateComboBox.Name = "BaudRateComboBox";
            this.BaudRateComboBox.SelectedIndexChanged += new System.EventHandler(this.BaudRateComboBox_SelectedIndexChanged);
            // 
            // label7
            // 
            resources.ApplyResources(this.label7, "label7");
            this.label7.Name = "label7";
            // 
            // label8
            // 
            resources.ApplyResources(this.label8, "label8");
            this.label8.Name = "label8";
            // 
            // label9
            // 
            resources.ApplyResources(this.label9, "label9");
            this.label9.Name = "label9";
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.Name = "label10";
            // 
            // NumberSendTextBox
            // 
            resources.ApplyResources(this.NumberSendTextBox, "NumberSendTextBox");
            this.NumberSendTextBox.Name = "NumberSendTextBox";
            this.NumberSendTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numberSendKeyPress);
            // 
            // label11
            // 
            resources.ApplyResources(this.label11, "label11");
            this.label11.Name = "label11";
            // 
            // SendIntervalTextBox
            // 
            resources.ApplyResources(this.SendIntervalTextBox, "SendIntervalTextBox");
            this.SendIntervalTextBox.Name = "SendIntervalTextBox";
            this.SendIntervalTextBox.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.sendIntervalKeyPress);
            // 
            // IDAutoIncCheckBox
            // 
            resources.ApplyResources(this.IDAutoIncCheckBox, "IDAutoIncCheckBox");
            this.IDAutoIncCheckBox.Name = "IDAutoIncCheckBox";
            this.IDAutoIncCheckBox.UseVisualStyleBackColor = true;
            this.IDAutoIncCheckBox.CheckedChanged += new System.EventHandler(this.IDAutoIncCheckBox_CheckedChanged);
            // 
            // DataAutoIncCheckBox
            // 
            resources.ApplyResources(this.DataAutoIncCheckBox, "DataAutoIncCheckBox");
            this.DataAutoIncCheckBox.Name = "DataAutoIncCheckBox";
            this.DataAutoIncCheckBox.UseVisualStyleBackColor = true;
            this.DataAutoIncCheckBox.CheckedChanged += new System.EventHandler(this.DataAutoIncCheckBox_CheckedChanged);
            // 
            // HideErrorFrameCheckBox
            // 
            resources.ApplyResources(this.HideErrorFrameCheckBox, "HideErrorFrameCheckBox");
            this.HideErrorFrameCheckBox.Name = "HideErrorFrameCheckBox";
            this.HideErrorFrameCheckBox.UseVisualStyleBackColor = true;
            this.HideErrorFrameCheckBox.CheckedChanged += new System.EventHandler(this.HideErrorFrameCheckBox_CheckedChanged);
            // 
            // DelayedSendBtn
            // 
            resources.ApplyResources(this.DelayedSendBtn, "DelayedSendBtn");
            this.DelayedSendBtn.Name = "DelayedSendBtn";
            this.DelayedSendBtn.UseVisualStyleBackColor = true;
            this.DelayedSendBtn.Click += new System.EventHandler(this.DelayedSendBtn_Click);
            // 
            // SendBtn
            // 
            resources.ApplyResources(this.SendBtn, "SendBtn");
            this.SendBtn.Name = "SendBtn";
            this.SendBtn.UseVisualStyleBackColor = true;
            this.SendBtn.Click += new System.EventHandler(this.SendBtn_Click);
            // 
            // ClearBtn
            // 
            resources.ApplyResources(this.ClearBtn, "ClearBtn");
            this.ClearBtn.Name = "ClearBtn";
            this.ClearBtn.UseVisualStyleBackColor = true;
            this.ClearBtn.Click += new System.EventHandler(this.ClearBtn_Click);
            // 
            // ExportBtn
            // 
            resources.ApplyResources(this.ExportBtn, "ExportBtn");
            this.ExportBtn.Name = "ExportBtn";
            this.ExportBtn.UseVisualStyleBackColor = true;
            this.ExportBtn.Click += new System.EventHandler(this.ExportBtn_Click);
            // 
            // label12
            // 
            resources.ApplyResources(this.label12, "label12");
            this.label12.Name = "label12";
            // 
            // LangCombox
            // 
            this.LangCombox.FormattingEnabled = true;
            this.LangCombox.Items.AddRange(new object[] {
            resources.GetString("LangCombox.Items"),
            resources.GetString("LangCombox.Items1")});
            resources.ApplyResources(this.LangCombox, "LangCombox");
            this.LangCombox.Name = "LangCombox";
            this.LangCombox.SelectedIndexChanged += new System.EventHandler(this.LangCombox_SelectedIndexChanged);
            // 
            // label13
            // 
            resources.ApplyResources(this.label13, "label13");
            this.label13.Name = "label13";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::InnoMakerUsb2Can.Properties.Resources._1691587198474__pic_hd;
            resources.ApplyResources(this.pictureBox1, "pictureBox1");
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Click += new System.EventHandler(this.pictureBox1_Click);
            // 
            // FrameIdTextBox
            // 
            resources.ApplyResources(this.FrameIdTextBox, "FrameIdTextBox");
            this.FrameIdTextBox.Name = "FrameIdTextBox";
            this.FrameIdTextBox.TextChanged += new System.EventHandler(this.FrameIdTextBox_TextChanged);
            this.FrameIdTextBox.MouseEnter += new System.EventHandler(this.FrameIdMouseEnter);
            this.FrameIdTextBox.MouseLeave += new System.EventHandler(this.FrameIdMouseLeave);
            // 
            // DataTextBox
            // 
            resources.ApplyResources(this.DataTextBox, "DataTextBox");
            this.DataTextBox.Name = "DataTextBox";
            this.DataTextBox.TextChanged += new System.EventHandler(this.DataTextBox_TextChanged);
            this.DataTextBox.MouseEnter += new System.EventHandler(this.DateTextBoxMouseEnter);
            this.DataTextBox.MouseLeave += new System.EventHandler(this.DateTextBoxMouseLeave);
            // 
            // listView
            // 
            this.listView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader4,
            this.columnHeader5,
            this.columnHeader6,
            this.columnHeader7,
            this.columnHeader8,
            this.columnHeader9,
            this.columnHeader10});
            this.listView.HideSelection = false;
            resources.ApplyResources(this.listView, "listView");
            this.listView.Name = "listView";
            this.listView.UseCompatibleStateImageBehavior = false;
            this.listView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            resources.ApplyResources(this.columnHeader1, "columnHeader1");
            // 
            // columnHeader2
            // 
            resources.ApplyResources(this.columnHeader2, "columnHeader2");
            // 
            // columnHeader3
            // 
            resources.ApplyResources(this.columnHeader3, "columnHeader3");
            // 
            // columnHeader4
            // 
            resources.ApplyResources(this.columnHeader4, "columnHeader4");
            // 
            // columnHeader5
            // 
            resources.ApplyResources(this.columnHeader5, "columnHeader5");
            // 
            // columnHeader6
            // 
            resources.ApplyResources(this.columnHeader6, "columnHeader6");
            // 
            // columnHeader7
            // 
            resources.ApplyResources(this.columnHeader7, "columnHeader7");
            // 
            // columnHeader8
            // 
            resources.ApplyResources(this.columnHeader8, "columnHeader8");
            // 
            // columnHeader9
            // 
            resources.ApplyResources(this.columnHeader9, "columnHeader9");
            // 
            // columnHeader10
            // 
            resources.ApplyResources(this.columnHeader10, "columnHeader10");
            // 
            // InnoMakerusb2Can
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.listView);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.LangCombox);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.ExportBtn);
            this.Controls.Add(this.ClearBtn);
            this.Controls.Add(this.SendBtn);
            this.Controls.Add(this.DelayedSendBtn);
            this.Controls.Add(this.HideErrorFrameCheckBox);
            this.Controls.Add(this.DataAutoIncCheckBox);
            this.Controls.Add(this.IDAutoIncCheckBox);
            this.Controls.Add(this.SendIntervalTextBox);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.NumberSendTextBox);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.DataTextBox);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.FrameIdTextBox);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.BaudRateComboBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.ModeComboBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.TypeComboBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.FormatComboBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.OpenDeviceBtn);
            this.Controls.Add(this.ScanDeviceBtn);
            this.Controls.Add(this.DeviceComboBox);
            this.Controls.Add(this.label1);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "InnoMakerusb2Can";
            this.Load += new System.EventHandler(this.InnoMakerusb2Can_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox DeviceComboBox;
        private System.Windows.Forms.Button ScanDeviceBtn;
        private System.Windows.Forms.Button OpenDeviceBtn;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox FormatComboBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox TypeComboBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox ModeComboBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox BaudRateComboBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private TextBoxEx FrameIdTextBox;
        private System.Windows.Forms.Label label9;
        private TextBoxEx DataTextBox;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox NumberSendTextBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox SendIntervalTextBox;
        private System.Windows.Forms.CheckBox IDAutoIncCheckBox;
        private System.Windows.Forms.CheckBox DataAutoIncCheckBox;
        private System.Windows.Forms.CheckBox HideErrorFrameCheckBox;
        private System.Windows.Forms.Button DelayedSendBtn;
        private System.Windows.Forms.Button SendBtn;
        private System.Windows.Forms.Button ClearBtn;
        private System.Windows.Forms.Button ExportBtn;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox LangCombox;
        private System.Windows.Forms.Label label13;
        private ToolTip toolTip1;
        private PictureBox pictureBox1;
    
        private ListView listView;
        private ColumnHeader columnHeader1;
        private ColumnHeader columnHeader2;
        private ColumnHeader columnHeader3;
        private ColumnHeader columnHeader4;
        private ColumnHeader columnHeader5;
        private ColumnHeader columnHeader6;
        private ColumnHeader columnHeader7;
        private ColumnHeader columnHeader8;
        private ColumnHeader columnHeader9;
        private ColumnHeader columnHeader10;
    }
}


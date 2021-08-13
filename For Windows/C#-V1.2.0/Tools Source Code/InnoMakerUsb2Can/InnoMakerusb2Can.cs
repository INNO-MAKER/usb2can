using NPOI.SS.UserModel;
using NPOI.XSSF.UserModel;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;
using Timer = System.Timers.Timer;

using InnoMakerUsb2CanLib;
using System.Collections.Concurrent;


namespace InnoMakerUsb2Can
{

    /// <summary>
    /// Frame Format
    /// </summary>
    enum FrameFormat
    {
        FrameFormatStandard = 0
    }


    /// <summary>
    /// Frame Type
    /// </summary>
    enum FrameType
    {
        FrameTypeData = 0
    }


    /// <summary>
    /// Can Mode
    /// </summary>
    enum CanMode
    {
        CanModeNormal = 0
    }


    /// <summary>
    /// Check ID Type
    /// </summary>
    enum CheckIDType
    {
        CheckIDTypeNone = 0,
        CheckIDTypeIncrease = 1
    }


    /// <summary>
    /// Check Data Type
    /// </summary>
    enum CheckDataType
    {
        CheckDataTypeNone = 0,
        CheckDataTypeIncrease = 1
    }


    /// <summary>
    /// Check Error Frame 
    /// </summary>
    enum CheckErrorFrame
    {
        CheckErrorFrameClose = 0,
        CheckErrorFrameOpen = 1
    }


    /// <summary>
    /// System Language
    /// </summary>
    enum SystemLanguage
    {
        DefaultLanguage = 0,
        EnglishLanguage = 1,
        ChineseLanguage = 2
    }


    public partial class InnoMakerusb2Can : Form
    {
        [DllImport("User32.dll")]
        public static extern void PostMessage(IntPtr hWnd, int msg, int wParam, ref My_lParam lParam);

        [DllImport("User32.dll")]
        private static extern Int32 SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
        public const int WM_VSCROLL = 0x0115;
        public const int SB_BOTTOM = 7;
        public const int USER = 0x0400;

        private CheckIDType checkIdType;
        private CheckDataType checkDataType;
        private CheckErrorFrame checkErrorFrame;
        private SystemLanguage currentLanguage;
        private UsbCan usbIO;
        InnoMakerDevice currentDeivce;
        /// <summary>
        ///  When Delayed Send, Record delayed send frame id 
        /// </summary>
        String delayedSendFrameId = "";

        /// <summary>
        /// When Delayed Send, Record delayed send frame data
        /// </summary>
        String delayedSendFrameData = "";

        /// <summary>
        /// When Delayed Send, number send time
        /// </summary>
        UInt32 numberSended = 0;
        UInt32 numberNeedToSend = 0;
        UsbCan.innomaker_can can;

        private String[] bitrateIndexes = {
                                "20K","33.33K","40K","50K",
                                "66.66K","80K", "83.33K","100K",
                                "125K","200K", "250K","400K",
                                 "500K","666K","800K", "1000K"
                              };

        private String[] workModeInexes =
        {
            "Normal","LoopBack","Repeat"
        };

        private String[] frameFormatIndexes =
        {
           "Standard","Extended"
        };

        private String[] frameTypeIndexes =
        {
            "Data Frame","Remote Frame"
        };

        public struct My_lParam
        {
            public int i;
            public string s;
        }

        private int curBitrateSelectIndex;
        private int curWorkModeSelectIndex;
        private int curFrameFormatSelectIndex;
        private int curFrameTypeSelectIndex;
        private Timer sendTimer;
        private Timer recvTimer;
        private Thread sendThread;
        private Thread recvThread;
        private Thread workerThread;
        int txNum = 0;
        int rxNum = 0;
        int rxErrorNum = 0;
        private bool sendThreadExitFlag;
        private bool recvThreadExitFlag;
        private bool workerThreadExitFlag;
        ConcurrentQueue<Byte[]> recvBufQueue = new ConcurrentQueue<Byte[]>();
        List<Byte[]> recBufList = new List<Byte[]>();

        public SpinLock recv_buf_lock;
        delegate void updateListViewDelegate(Byte[] inputBytes);
        delegate void updateSendBtnDelegate(int tag);
        delegate void updateListBoxDelegate(String str);
        public InnoMakerusb2Can()
        {
            InitializeComponent();
        }



        private void InnoMakerusb2Can_Load(object sender, EventArgs e)
        {
 
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.AllPaintingInWmPaint, true); // 禁止擦除背景.
            SetStyle(ControlStyles.DoubleBuffer, true);

            usbIO = new UsbCan();
            usbIO.removeDeviceDelegate = this.RemoveDeviceNotifyDelegate;
            usbIO.addDeviceDelegate = this.AddDeviceNotifyDelegate;

 
            can = new UsbCan.innomaker_can();
            can.tx_context = new UsbCan.innomaker_tx_context[UsbCan.innomaker_MAX_TX_URBS];


            checkIdType = CheckIDType.CheckIDTypeNone;
            checkErrorFrame = CheckErrorFrame.CheckErrorFrameClose;

            NumberSendTextBox.Text = "1";
            SendIntervalTextBox.Text = "1000";

            curBitrateSelectIndex = -1;
            curWorkModeSelectIndex = -1;
            curFrameFormatSelectIndex = 0;
            curFrameTypeSelectIndex = 0;

            FrameIdTextBox.Text = "00 00 00 00";
            FrameIdTextBox.PlaceHolderText = "Please Input Four Bytes Hex";
            DataTextBox.Text = "00 00 00 00 00 00 00 00";
            DataTextBox.PlaceHolderText = "Please Input Eight Bytes Hex";

            BaudRateComboBox.DataSource = bitrateIndexes;
            ModeComboBox.DataSource = workModeInexes;
            FormatComboBox.DataSource = frameFormatIndexes;
            TypeComboBox.DataSource = frameTypeIndexes;

            FormatComboBox.Enabled = true;
            TypeComboBox.Enabled = true;

            OpenDeviceBtn.Tag = 0;
            DelayedSendBtn.Tag = 0;



            /// Read local language , if null, use english default, else use local language
            String lan = Properties.Settings.Default.Language;
            if (lan == "English")
            {
                currentLanguage = SystemLanguage.EnglishLanguage;
                LangCombox.Text = "English";
                Thread.CurrentThread.CurrentUICulture = CultureInfo.GetCultureInfo("en-US");
                ApplyResources();
            }
            else
            {
                currentLanguage = SystemLanguage.ChineseLanguage;
                LangCombox.Text = "Chinese";
                Thread.CurrentThread.CurrentUICulture = CultureInfo.GetCultureInfo("zh-Hans");
                ApplyResources();
            }



        }

        private void DeviceComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            currentDeivce = usbIO.getInnoMakerDevice(DeviceComboBox.SelectedIndex);
        }

        private void ScanDeviceBtn_Click(object sender, EventArgs e)
        {

            usbIO.closeInnoMakerDevice(currentDeivce);
            currentDeivce = null;

            DeviceComboBox.Text = "";
            usbIO.scanInnoMakerDevices();
            UpdateDevices();
        }

        private void OpenDeviceBtn_Click(object sender, EventArgs e)
        {
            if (OpenDeviceBtn.Tag.Equals(1))
            {
                _closeDevice();
            }
            else
            {
                _openDevice();

            }
        }


        private void ModeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            curWorkModeSelectIndex = ModeComboBox.SelectedIndex;
        }

        private void BaudRateComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            curBitrateSelectIndex = BaudRateComboBox.SelectedIndex;
        }

        private void DelayedSendBtn_Click(object sender, EventArgs e)
        {

            /// If running , stop
            if (DelayedSendBtn.Tag.Equals(1))
            {
                stopSendTimerThread();
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    DelayedSendBtn.Text = "定时发送";
                }
                else
                {
                    DelayedSendBtn.Text = "Delayed Send";
                }

                DelayedSendBtn.Tag = 0;
            }
            /// If not running ,restart
            else
            {
                stopSendTimerThread();
                startSendTimerThread();
            }
        }

        private void SendBtn_Click(object sender, EventArgs e)
        {
            String frameId = FrameIdTextBox.Text;
            String frameData = DataTextBox.Text;


            if (currentDeivce == null || currentDeivce.isOpen == false)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("设备未打开");
                }
                else
                {
                    MessageBox.Show("Device Not Open");
                }
                return;
            }

            if (curBitrateSelectIndex == -1)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("波特率不正确");
                }
                else
                {
                    MessageBox.Show("BaudRate Not Right");
                }
                return;
            }

            if (curWorkModeSelectIndex == -1)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("设备模式不正确");
                }
                else
                {
                    MessageBox.Show("Work Mode Not Right");
                }
                return;
            }

            if (frameId.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("帧ID不正确");
                }
                else
                {
                    MessageBox.Show("Frame ID Not Right");
                }
                return;
            }

            if (frameData.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("帧数据不正确");
                }
                else
                {
                    MessageBox.Show("Frame Data Not Right");
                }
                return;
            }

            /* find an empty context to keep track of transmission */
            UsbCan.innomaker_tx_context txc = UsbCan.innomaker_alloc_tx_context(can);
            if (txc.echo_id == 0xff)
            {
                Console.WriteLine("SEND FAIL: NETDEV_BUSY");
                return;
            }

            Byte[] standardFrameData = buildStandardFrame(frameId, frameData, txc.echo_id);
            int transferOut = 0;
            bool result = usbIO.syncSendInnoMakerDeviceBuf(currentDeivce, standardFrameData, standardFrameData.Length, 10, transferOut);
            if (result)
            {
                Console.WriteLine("SEND SUCCESS:" + getHexString(standardFrameData));
            }
            else
            {
                Console.WriteLine("SEND FAIL:" + getHexString(standardFrameData));
                UsbCan.innomaker_tx_context txc1 = UsbCan.innomaker_get_tx_context(can, txc.echo_id);
                if (txc1 != null)
                {
                    UsbCan.innomaker_free_tx_context(txc1);
                }
            }
        }
    
        public static String getHexString(byte[] b)
        {
            String hex = "0x|";
            for (int i = 0; i < b.Length; i++)
            {
                hex += (b[i] & 0xFF).ToString("X2");

                if (hex.Length == 1)
                {
                    hex = '0' + hex;
                }

                hex += " ";

            }

            return hex.ToUpper();

        }

        private void ClearBtn_Click(object sender, EventArgs e)
        {

            this.listView.Items.Clear();
            rxNum = 0;
            txNum = 0;
            rxErrorNum = 0;
        }


        public void ExportExcel(ListView lv)
        {
            if (lv.Items == null) return;

            string saveFileName = "";
            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.DefaultExt = "xls";
            saveDialog.Filter = "Excel File|*.xls";
            saveDialog.FileName = DateTime.Now.ToString("yyyy-MM-dd");
            saveDialog.ShowDialog();
            saveFileName = saveDialog.FileName;
            if (saveFileName.IndexOf(":") < 0)
                return;

            if (File.Exists(saveFileName)) File.Delete(saveFileName);

            DoExport(this.listView, saveFileName);
        }

        private void ExportBtn_Click(object sender, EventArgs e)
        {
            ExportExcel(this.listView);
        }

        private void IDAutoIncCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            checkIdType = IDAutoIncCheckBox.Checked ? CheckIDType.CheckIDTypeIncrease : CheckIDType.CheckIDTypeNone;
        }

        private void DataAutoIncCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            checkDataType = DataAutoIncCheckBox.Checked ? CheckDataType.CheckDataTypeIncrease : CheckDataType.CheckDataTypeNone;
        }

        private void HideErrorFrameCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            checkErrorFrame = HideErrorFrameCheckBox.Checked ? CheckErrorFrame.CheckErrorFrameOpen : CheckErrorFrame.CheckErrorFrameClose;
        }

        private void _openDevice()
        {

            if (currentDeivce == null)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("设备未打开，请选择设备");
                }
                else
                {
                    MessageBox.Show("Device not open, Please select device");
                }
                return;
            }

            if (curBitrateSelectIndex == -1)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("波特率不正确");
                }
                else
                {
                    MessageBox.Show("Baudrate not right");
                }
                return;
            }

            UsbCan.UsbCanMode usbCanMode = UsbCan.UsbCanMode.UsbCanModeNormal;
            if (curWorkModeSelectIndex == 0)
            {
                usbCanMode = UsbCan.UsbCanMode.UsbCanModeNormal;
            }
            else if (curWorkModeSelectIndex == 1)
            {
                usbCanMode = UsbCan.UsbCanMode.UsbCanModeLoopback;
            }
            else if (curWorkModeSelectIndex == 2)
            {
                usbCanMode = UsbCan.UsbCanMode.UsbCanModeListenOnly;
            }

            UsbCan.innomaker_device_bittming deviceBittming = GetBittming(BaudRateComboBox.SelectedIndex);

            bool result = usbIO.UrbSetupDevice(currentDeivce, usbCanMode, deviceBittming);
            if (!result)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("打开设备失败，请重新选择或扫描设备再打开!，若还不行，重新插拔");
                }
                else
                {
                    MessageBox.Show("Open Device Fail, Please Reselect Or Scan Device and Open, If alse fail, replug in device");
                }

                return;
            }
            /// Reset current device tx_context  
            for (int i = 0; i < UsbCan.innomaker_MAX_TX_URBS; i++)
            {
                can.tx_context[i] = new UsbCan.innomaker_tx_context();
                can.tx_context[i].echo_id = UsbCan.innomaker_MAX_TX_URBS;
            }

            startRecvTimerThread();
            startWorkerThread();
            OpenDeviceBtn.Tag = 1;
            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                OpenDeviceBtn.Text = "关闭设备";
            }
            else
            {
                OpenDeviceBtn.Text = "Close Device";
            }

            BaudRateComboBox.Enabled = false;
            DeviceComboBox.Enabled = false;
            ModeComboBox.Enabled = false;
            ScanDeviceBtn.Enabled = false;
        }

        private void _closeDevice()
        {
       
            stopSendTimerThread();
            stopRecvTimerThread();
            stopWorkerThread();
            /// Wait for recv timer process done, because recv time interval is 100
            Thread.Sleep(100);
            if (currentDeivce != null && currentDeivce.isOpen == true)
            {
                usbIO.UrbResetDevice(currentDeivce);
                usbIO.closeInnoMakerDevice(currentDeivce);

            }

            /// Reset current device tx_context
            for (int i = 0; i < UsbCan.innomaker_MAX_TX_URBS; i++)
            {
                can.tx_context[i] = new UsbCan.innomaker_tx_context();
                can.tx_context[i].echo_id = UsbCan.innomaker_MAX_TX_URBS;
            }

            BaudRateComboBox.Enabled = true;
            DeviceComboBox.Enabled = true;
            ScanDeviceBtn.Enabled = true;
            ModeComboBox.Enabled = true;
            OpenDeviceBtn.Tag = 0;
            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                OpenDeviceBtn.Text = "打开设备";

            }
            else
            {
                OpenDeviceBtn.Text = "Open Device";
            }
            DelayedSendBtn.Tag = 0;
            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                DelayedSendBtn.Text = "定时发送";
            }
            else
            {
                DelayedSendBtn.Text = "Delayed Send";
            }
        }

        private UsbCan.innomaker_device_bittming GetBittming(int index)
        {
            UsbCan.innomaker_device_bittming bittming;

            switch (index)
            {
                case 0: // 20K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 150;
                    break;
                case 1: // 33.33K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 1;
                    bittming.sjw = 1;
                    bittming.brp = 180;
                    break;
                case 2: // 40K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 75;
                    break;
                case 3: // 50K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 60;
                    break;
                case 4: // 66.66K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 1;
                    bittming.sjw = 1;
                    bittming.brp = 90;
                    break;
                case 5: // 80K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 1;
                    bittming.sjw = 1;
                    bittming.brp = 75;
                    break;
                case 6: // 83.33K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 1;
                    bittming.sjw = 1;
                    bittming.brp = 72;
                    break;


                case 7: // 100K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 30;
                    break;
                case 8: // 125K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 24;
                    break;
                case 9: // 200K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 15;
                    break;
                case 10: // 250K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 12;
                    break;
                case 11: // 400K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 1;
                    bittming.sjw = 1;
                    bittming.brp = 15;
                    break;
                case 12: // 500K
                    bittming.prop_seg = 6;
                    bittming.phase_seg1 = 7;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 6;
                    break;
                case 13: // 666K
                    bittming.prop_seg = 3;
                    bittming.phase_seg1 = 3;
                    bittming.phase_seg2 = 2;
                    bittming.sjw = 1;
                    bittming.brp = 8;
                    break;
                case 14: /// 800K
                    bittming.prop_seg = 7;
                    bittming.phase_seg1 = 8;
                    bittming.phase_seg2 = 4;
                    bittming.sjw = 1;
                    bittming.brp = 3;
                    break;
                case 15: /// 1000K
                    bittming.prop_seg = 5;
                    bittming.phase_seg1 = 6;
                    bittming.phase_seg2 = 4;
                    bittming.sjw = 1;
                    bittming.brp = 3;
                    break;
                default: /// 1000K
                    bittming.prop_seg = 5;
                    bittming.phase_seg1 = 6;
                    bittming.phase_seg2 = 4;
                    bittming.sjw = 1;
                    bittming.brp = 3;
                    break;
            }
            return bittming;
        }

        public void ShowMessage(string msg)

        {

            this.Invoke(new MessageBoxShow(MessageBoxShow_F), new object[] {msg
    });

        }

        delegate void MessageBoxShow(string msg);

        void MessageBoxShow_F(string msg)

        {

            MessageBox.Show(msg, "提示信息", MessageBoxButtons.OK, MessageBoxIcon.Information);

        }
        void AddDeviceNotifyDelegate()
        {
            /// Close current device 
            if (currentDeivce != null)
            {
                _closeDevice();
                DeviceComboBox.Text = "";
                currentDeivce = null;
            }
            usbIO.scanInnoMakerDevices();
            UpdateDevices();
        }
        void RemoveDeviceNotifyDelegate()
        {
            /// Close current device 
            if (currentDeivce != null)
            {
                _closeDevice();
                DeviceComboBox.Text = "";
                currentDeivce = null;
            }

            usbIO.scanInnoMakerDevices();
            UpdateDevices();
        }

        void UpdateDevices()
        {
            List<String> devIndexes = new List<string>();
            for (int i = 0; i < usbIO.getInnoMakerDeviceCount(); i++)
            {
                InnoMakerDevice device = usbIO.getInnoMakerDevice(i);
                devIndexes.Add(device.deviceId);
            }

            DeviceComboBox.DataSource = devIndexes;
        }


        Byte[] buildStandardFrame(String frameId, String frameData, uint echoId)
        {

            /// Format frame id
            frameId = Formatter.formatStringToHex(frameId, 8, true);
            /// Format frame data
            frameData = Formatter.formatStringToHex(frameData, 16, true);
            /// Data Length
            int dlc = frameData.Split(' ').Length;

            UsbCan.innomaker_host_frame frame;

            frame.data = new Byte[8];
            frame.echo_id = echoId;
            frame.can_dlc = (byte)dlc;
            frame.channel = 0;
            frame.flags = 0;
            frame.reserved = 0;

            String[] canIdArr = frameId.Split(' ');
            Byte[] canId = { 0x00, 0x00, 0x00, 0x00 };
            for (int i = 0; i < canIdArr.Length; i++)
            {
                String b = canIdArr[canIdArr.Length - i - 1];
                canId[i] = (Byte)Convert.ToInt32(b, 16);

            }
            frame.can_id = System.BitConverter.ToUInt32(canId, 0);

            String[] dataByte = frameData.Split(' ');
            for (int i = 0; i < dataByte.Length; i++)
            {
                String byteValue = dataByte[i];

                frame.data[i] = (Byte)Convert.ToInt32(byteValue, 16);
            }

            if (curFrameFormatSelectIndex == 1)
            {
                frame.can_id |= usbIO.Can_Eff_Flag;
            }

            if (curFrameTypeSelectIndex == 1)
            {
                frame.can_id |= usbIO.Can_Rtr_Flag;
            }

            frame.timestamp_us = 0;

            return StructureHelper.StructToBytes(frame);

        }



        void setupSendTimer(int interval)
        {
            numberSended = 0;
            sendThreadExitFlag = false;
            while (!sendThreadExitFlag)
            {
                sendToDev(null, null);
                Thread.Sleep(interval);

            }
        }


        void sendToDev(object source, System.Timers.ElapsedEventArgs e)
        {

            /* find an empty context to keep track of transmission */
            UsbCan.innomaker_tx_context txc = UsbCan.innomaker_alloc_tx_context(can);
            if (txc.echo_id == 0xff)
            {
                Console.WriteLine("SEND FAIL: NETDEV_BUSY");
                return;
            }

            Byte[] standardFrameData = buildStandardFrame(delayedSendFrameId, delayedSendFrameData, txc.echo_id);
            int transferOut = 0;
            bool result = usbIO.syncSendInnoMakerDeviceBuf(currentDeivce, standardFrameData, standardFrameData.Length, 10, transferOut);
            if (result)
            {
                Console.WriteLine("SEND SUCCESS:" + getHexString(standardFrameData));

            }
            else
            {
                UsbCan.innomaker_tx_context txc1 = UsbCan.innomaker_get_tx_context(can, txc.echo_id);
                if (txc1 != null)
                {
                    UsbCan.innomaker_free_tx_context(txc1);
                }
                Console.WriteLine("SEND FAIL:" + getHexString(standardFrameData));
                return;
            }

            if (checkIdType == CheckIDType.CheckIDTypeNone)
            {

            }
            else
            {
                /// Increase Frame ID
                delayedSendFrameId = increaseFrameIdHexString(delayedSendFrameId, 8);
            }

            if (checkDataType == CheckDataType.CheckDataTypeNone)
            {
            }
            else
            {
                ///Increase Frame Data;
                String frameData = Formatter.formatStringToHex(delayedSendFrameData, 16, true);
                int dlc = frameData.Split(' ').Length;
                delayedSendFrameData = increaseFrameIdHexString(delayedSendFrameData, dlc * 2);
            }

            if (++numberSended == numberNeedToSend)
            {
                updateSendBtnDelegate d = new updateSendBtnDelegate(updateSendBtn);
                this.Invoke(d, 0);
                stopSendTimerThread();

                return;
            }

        }

        // Start Send Thread
        void startSendTimerThread()
        {
            if (sendThread != null)
            {
                sendThread.Abort();
                sendThread = null;
            }

            int interval = int.Parse(SendIntervalTextBox.Text);

            if (interval < 1 || interval > 5000)
            {
                return;
            }

            if (int.Parse(NumberSendTextBox.Text) < 1 || int.Parse(NumberSendTextBox.Text) > 5000000)
            {
                return;
            }

            String frameId = FrameIdTextBox.Text;
            String frameData = DataTextBox.Text;

            if (currentDeivce == null || currentDeivce.isOpen == false)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {

                    ShowMessage("设备未打开");
                }
                else
                {
                    ShowMessage("Device Not Open");
                }
                return;
            }

            if (curBitrateSelectIndex == -1)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    ShowMessage("波特率不正确");
                }
                else
                {
                    ShowMessage("BaudRate Not Right");
                }
                return;
            }

            if (curWorkModeSelectIndex == -1)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    ShowMessage("设备模式不正确");
                }
                else
                {
                    ShowMessage("Work Mode Not Right");
                }
                return;
            }

            if (frameId.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    ShowMessage("帧ID不正确");
                }
                else
                {
                    ShowMessage("Frame ID Not Right");
                }
                return;
            }

            if (frameData.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    MessageBox.Show("帧数据不正确");
                }
                else
                {
                    MessageBox.Show("Frame Data Not Right");
                }
                return;
            }


            if (delayedSendFrameId.Length == 0)
            {
                delayedSendFrameId = FrameIdTextBox.Text;
            }
            if (delayedSendFrameData.Length == 0)
            {
                delayedSendFrameData = DataTextBox.Text;
            }

            if (delayedSendFrameId.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    DelayedSendBtn.Text = "定时发送";
                    MessageBox.Show("帧ID不正确");
                }
                else
                {
                    DelayedSendBtn.Text = "Delayed Send";
                    MessageBox.Show("Frame ID Not Right");
                }

                DelayedSendBtn.Tag = 0;
                return;
            }

            if (delayedSendFrameData.Length == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    DelayedSendBtn.Text = "定时发送";
                    MessageBox.Show("帧数据不正确");
                }
                else
                {
                    DelayedSendBtn.Text = "Delayed Send";
                    MessageBox.Show("Frame Data Not Right");
                }
                DelayedSendBtn.Tag = 0;
                return;
            }

            updateSendBtn(1);

            Console.WriteLine("Start send timer thread");
            sendThread = new Thread(sendTimerThreadMain);
            sendThread.Start();

        }

        void sendTimerThreadMain()
        {
            int interval = int.Parse(SendIntervalTextBox.Text);
            numberNeedToSend = UInt32.Parse(NumberSendTextBox.Text);
            setupSendTimer(interval);
        }

        void stopSendTimerThread()
        {
            Console.WriteLine("Stop send timer thread");
            if (sendThread != null)
            {
                cancelSendTimer();
                sendThreadExitFlag = true;
                sendThread.Abort();
                sendThread = null;
            }

        }

        void startWorkerThread()
        {

            if (workerThread != null)
            {
                workerThread.Abort();
                workerThread = null;
            }
            Console.WriteLine("Start worker thread");

            workerThread = new Thread(workerThreadMain);
            workerThread.Start();
        }

        void workerThreadMain()
        {
            workerThreadExitFlag = false;
            while (!workerThreadExitFlag)
            {
                if (this.recBufList.Count > 0)
                {
                    bool _lock = false;
                    recv_buf_lock.Enter(ref _lock);
                    Byte[][] copyList = new Byte[recBufList.Count][];
                    this.recBufList.CopyTo(copyList);
                    this.recBufList.Clear();

                    recv_buf_lock.Exit();
                    _lock = false;

                    this.listView.Invoke(new Action(() =>
                    {

                        List<ListViewItem> addListItems = new List<ListViewItem>();
                        for (int i = 0; i < copyList.Length; i++)
                        {
                            ListViewItem item = getListViewItem(copyList[i]);
                            if (item != null)
                                addListItems.Add(item);
                        }

                        this.listView.BeginUpdate();
                        this.listView.Items.AddRange(addListItems.ToArray());
                        this.listView.EndUpdate();
                        SendMessage(listView.Handle, WM_VSCROLL, SB_BOTTOM, 0);
                    }));
                    Thread.Sleep(500);
                }
            }
        }

        void stopWorkerThread()
        {
            Console.WriteLine("Stop worker thread");
            if (workerThread != null)
            {
                workerThreadExitFlag = true;
                workerThread.Abort();
                workerThread = null;
            }
 
            bool _lock = false;
            try
            {
                recv_buf_lock.Enter(ref _lock);
                this.recBufList.Clear();
            }
            finally
            {
                if (_lock)
                    recv_buf_lock.Exit();
            }

        }

        // Start Recv Thread
        void startRecvTimerThread()
        {

            if (recvThread != null)
            {
                recvThread.Abort();
                recvThread = null;
            }
            Console.WriteLine("Start recv time thread");
            recvThread = new Thread(recvTimeThreadMain);
            recvThread.Start();
        }



        void recvTimeThreadMain()
        {
            setupRecvTimer();
        }

        void stopRecvTimerThread()
        {
            Console.WriteLine("Stop recv timer thread");
            if (recvThread != null)
            {
                cancelRecvTimer();
                recvThreadExitFlag = true;
                recvThread.Abort();
                recvThread = null;
            }
        }

        String increaseFrameIdHexString(String frameId, int bit)
        {
            String increaseFrameId = "";

            frameId = Formatter.formatStringToHex(frameId, bit, true);
            String[] dataByte = frameId.Split(' ');
            Byte[] frameIdBytes = new Byte[dataByte.Length];
            bool increaseBit = true;

            for (int i = dataByte.Length - 1; i >= 0; i--)
            {
                String byteValue = dataByte[i];
                frameIdBytes[i] = Convert.ToByte(byteValue, 16);
                if (increaseBit)
                {
                    if (frameIdBytes[i] + 1 > 0xff)
                    {
                        frameIdBytes[i] = 0x00;
                        increaseBit = true;
                    }
                    else
                    {
                        frameIdBytes[i] = (Byte)(frameIdBytes[i] + 1);
                        increaseBit = false;
                    }
                }
            }

            for (int i = 0; i < dataByte.Length; i++)
            {
                increaseFrameId += frameIdBytes[i].ToString("X2");
                if (i != dataByte.Length - 1)
                {
                    increaseFrameId += ' ';
                }
            }

            return increaseFrameId;
        }

        void setupRecvTimer()
        {
            recvThreadExitFlag = false;
            while (!recvThreadExitFlag)
            {
                inputFromDev(null, null);

            }
        }

        void inputFromDev(object source, System.Timers.ElapsedEventArgs e)
        {

            if (currentDeivce == null || currentDeivce.isOpen == false)
            {
                stopRecvTimerThread();
                stopWorkerThread();
                return;
            }

            UsbCan.innomaker_host_frame frame = new UsbCan.innomaker_host_frame();
            int size = Marshal.SizeOf(frame);
            Byte[] inputBytes = new Byte[size];
            int transferIn = 0;
            bool result = usbIO.syncGetInnoMakerDeviceBuf(currentDeivce, inputBytes, size, transferIn, int.MaxValue);
            if (result)
            {

                // Echo ID
                UInt32 echoId = BitConverter.ToUInt32(inputBytes, 0);


                /// Not recv frame 
                if (echoId != 0xffffffff)
                {
                    UsbCan.innomaker_tx_context txc = UsbCan.innomaker_get_tx_context(can, echoId);
                    ///bad devices send bad echo_ids.
                    if (txc == null)
                    {
                        Console.WriteLine("RECV FAIL:Bad Devices Send Bad Echo_ids");
                        return;
                    }
                    /// Free 
                    UsbCan.innomaker_free_tx_context(txc);
                }
 
                bool _lock = false;
                try
                {
 
                    recv_buf_lock.Enter(ref _lock);
 
                    recBufList.Add(inputBytes);
                }
                finally
                {
                    if (_lock)
                        recv_buf_lock.Exit();
                }

            }
        }

        void updateSendBtn(int tag)
        {
            if (tag == 0)
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    DelayedSendBtn.Text = "定时发送";
                }
                else
                {
                    DelayedSendBtn.Text = "Delayed Send";
                }

                DelayedSendBtn.Tag = 0;
            }
            else
            {
                if (currentLanguage == SystemLanguage.ChineseLanguage)
                {
                    DelayedSendBtn.Text = "停止";
                }
                else
                {
                    DelayedSendBtn.Text = "Stop";
                }

                DelayedSendBtn.Tag = 1;
            }
        }

        protected override void DefWndProc(ref System.Windows.Forms.Message m)
        {
            switch (m.Msg)
            {
                case USER + 1:
                    break;
                default:
                    base.DefWndProc(ref m);
                    break;
            }
        }
 

        ListViewItem getListViewItem(Byte[] inputBytes)
        {

            // Echo ID
            UInt32 echoId = BitConverter.ToUInt32(inputBytes, 0);
            /// Frame ID
            UInt32 frameId = BitConverter.ToUInt32(inputBytes, 4);

            byte dlc = inputBytes[8];

            Byte[] frameDataBytes = new Byte[dlc];
            Buffer.BlockCopy(inputBytes, 12, frameDataBytes, 0, dlc);

         
            // System time
            UInt32 timeStamp = BitConverter.ToUInt32(inputBytes, 20);

            String systemTimeStr = String.Format("{0:D3}.{1:D3}.{2:D3}", timeStamp / 1000000, (timeStamp % 1000000) / 1000, timeStamp % 1000);
            String channelStr = "0";
            if (rxNum > 4999999)
            {
                rxNum = 0;
            }
            if (txNum > 4999999)
            {
                txNum = 0;
            }
            if (rxErrorNum > 4999999)
            {
                rxErrorNum = 0;
            }


            String frameIdStr = "0x" + (frameId & usbIO.Can_Id_Mask).ToString("X2");
            String frameLengthStr = dlc.ToString();
            String frameDataStr = getHexString(frameDataBytes);

            String directionStr = "";
            String frameFormatStr = "";
            String frameTypeStr = "";

            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                directionStr = echoId == 0xffffffff ? "接受[" + rxNum.ToString() + "]" : "发送[" + txNum.ToString() + "]";
                frameFormatStr = ((frameId & usbIO.Can_Eff_Flag) != 0x00) ? "扩展帧" : "标准帧";
                frameTypeStr = ((frameId & usbIO.Can_Rtr_Flag) != 0x00) ? "远程帧" : "数据帧";
            }
            else
            {
                directionStr = echoId == 0xffffffff ? "Recv[" + rxNum.ToString() + "]" : "Send[" + txNum.ToString() + "]";
                frameFormatStr = ((frameId & usbIO.Can_Eff_Flag) != 0x00) ? "Extend Frame" : "Standard Frame";
                frameTypeStr = ((frameId & usbIO.Can_Rtr_Flag) != 0x00) ? "Remote Frame" : "Data Frame";

            }

            /// Means recv from remote can device
            if (echoId == 0xffffffff)
            {
                /// Hide error frame (Little Endian)
                if ((frameId & 0x00000020) == 0x20000000 && checkErrorFrame == CheckErrorFrame.CheckErrorFrameOpen)
                {
                    return null;
                }

                /// Seq 
              
                ListViewItem lvi = new ListViewItem();
                lvi.Text = (txNum + rxNum + rxErrorNum).ToString();
                lvi.ForeColor = ((frameId & usbIO.Can_Err_Flag) != 0x00) ? System.Drawing.Color.Red : System.Drawing.Color.Green;
                lvi.SubItems.Add(systemTimeStr);
                lvi.SubItems.Add(channelStr);
                lvi.SubItems.Add(directionStr);
                lvi.SubItems.Add(frameIdStr);
                lvi.SubItems.Add(frameTypeStr);
                lvi.SubItems.Add(frameFormatStr);
                lvi.SubItems.Add(frameLengthStr);
                lvi.SubItems.Add(frameDataStr);

                /// If Error Frame
                if ((frameId & usbIO.Can_Err_Flag) != 0x00)
                {
                  
                    if (currentLanguage == SystemLanguage.ChineseLanguage)
                    {
                        directionStr = "接受錯誤[" + rxErrorNum++.ToString() + "]";
                    }
                    else
                    {
                        directionStr =  "Recv Error[" + rxErrorNum++.ToString() + "]";
                    }

                    lvi.SubItems[3].Text = directionStr;
                    if ((frameId & usbIO.Can_Err_Restarted) != 0x00)
                    {
                        lvi.SubItems.Add("Fail: CAN_STATE_ERROR_ACTIVE");

                    }

                    else if ((frameId & usbIO.Can_Err_BusOff) != 0x00)
                    {
                        lvi.SubItems.Add("Fail: CAN_STATE_BUS_OFF");


                    }

                    else if ((frameId & usbIO.Can_Err_Ctrl) != 0x00)
                    {
                        if ((frameDataBytes[1] & usbIO.Can_Err_Ctrl_Tx_Warning) != 0x00 ||
                            (frameDataBytes[1] & usbIO.Can_Err_Ctrl_Rx_Warning) != 0x00)
                        {
                            lvi.SubItems.Add("Fail: CAN_STATE_ERROR_WARNING");

                        }
                        else if ((frameDataBytes[1] & usbIO.Can_Err_Ctrl_Tx_Passive) != 0x00 ||
                          (frameDataBytes[1] & usbIO.Can_Err_Ctrl_Rx_Passive) != 0x00)
                        {
                            lvi.SubItems.Add("Fail: CAN_STATE_ERROR_PASSIVE");

                        }
                        else if ((frameDataBytes[1] & usbIO.Can_Err_Ctrl_Tx_Overflow) != 0x00 ||
                         (frameDataBytes[1] & usbIO.Can_Err_Ctrl_Rx_Overflow) != 0x00)
                        {
                            lvi.SubItems.Add("Fail: CAN_STATE_ERROR_OVERFLOW");

                        }
                        else
                        {
                            lvi.SubItems.Add("Fail: Other Reason");

                        }

                    }
                }
                else
                {
                    lvi.SubItems.Add("Success");
                    rxNum++;
                }
               
                return lvi;
            }
            else // Send Frame
            {

              
                ListViewItem lvi = new ListViewItem();
                lvi.ForeColor = System.Drawing.Color.Black;
                lvi.Text = (txNum + rxNum + rxErrorNum).ToString();
                lvi.SubItems.Add(systemTimeStr);
                lvi.SubItems.Add(channelStr);
                lvi.SubItems.Add(directionStr);
                lvi.SubItems.Add(frameIdStr);
                lvi.SubItems.Add(frameTypeStr);
                lvi.SubItems.Add(frameFormatStr);
                lvi.SubItems.Add(frameLengthStr);
                lvi.SubItems.Add(frameDataStr);
                lvi.SubItems.Add("Success");
                txNum++;
                return lvi;

            }
        }



        void cancelRecvTimer()
        {
            if (recvTimer != null)
            {
                recvTimer.Enabled = false;
                recvTimer.Stop();
                recvTimer = null;
            }
        }

        void cancelSendTimer()
        {
            if (sendTimer != null)
            {
                sendTimer.Enabled = false;
                sendTimer.Stop();
                sendTimer = null;
            }

            delayedSendFrameId = "";
            delayedSendFrameData = "";
            numberSended = 0;
        }


        private void DoExport(ListView listView, string strFileName)
        {

            int rowNum = listView.Items.Count;
            int columnNum = listView.Items[0].SubItems.Count;
            int columnIndex = 0;

            if (rowNum == 0 || string.IsNullOrEmpty(strFileName))
            {
                return;
            }

            if (rowNum > 0)
            {
                XSSFWorkbook workBook = new XSSFWorkbook();
                XSSFSheet sheet = (XSSFSheet)workBook.CreateSheet();

                IRow frow0 = sheet.CreateRow(0);
                foreach (ColumnHeader dc in listView.Columns)
                {
                    frow0.CreateCell(columnIndex).SetCellValue(dc.Text);
                    columnIndex++;
                }

                for (int i = 0; i < rowNum; i++)
                {
                    IRow frow1 = sheet.CreateRow(i + 1);
                    for (int j = 0; j < columnNum; j++)
                    {
                        frow1.CreateCell(j).SetCellValue(listView.Items[i].SubItems[j].Text);

                    }
                }
                try
                {
                    using (FileStream fs = new FileStream(strFileName, FileMode.Create, FileAccess.Write))
                    {
                        workBook.Write(fs);
                        workBook.Close();
                    }
                    if (currentLanguage == SystemLanguage.ChineseLanguage)
                    {
                        MessageBox.Show("导出成功");
                    }
                    else
                    {
                        MessageBox.Show("Export Success");
                    }

                }
                catch (Exception)
                {
                    workBook.Close();
                }

            }

        }


        private void numberSendKeyPress(object sender, KeyPressEventArgs e)
        {
            if (!Char.IsNumber(e.KeyChar) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void sendIntervalKeyPress(object sender, KeyPressEventArgs e)
        {
            if (!Char.IsNumber(e.KeyChar) && e.KeyChar != (char)8)
            {
                e.Handled = true;
            }
        }

        private void FrameIdTextBox_TextChanged(object sender, EventArgs e)
        {
           
        }

        private void DataTextBox_TextChanged(object sender, EventArgs e)
        {
         
        }

        public class StructureHelper
        {
            public static byte[] StructToBytes(object structObj)
            {

                int size = Marshal.SizeOf(structObj);
                IntPtr buffer = Marshal.AllocHGlobal(size);
                try
                {
                    Marshal.StructureToPtr(structObj, buffer, false);
                    byte[] bytes = new byte[size];
                    Marshal.Copy(buffer, bytes, 0, size);
                    return bytes;
                }
                finally
                {
                    Marshal.FreeHGlobal(buffer);
                }
            }

            public static object ByteToStruct(byte[] bytes, Type type)
            {
                int size = Marshal.SizeOf(type);
                if (size > bytes.Length)
                {
                    return null;
                }

                IntPtr structPtr = Marshal.AllocHGlobal(size);
                Marshal.Copy(bytes, 0, structPtr, size);
                object obj = Marshal.PtrToStructure(structPtr, type);
                Marshal.FreeHGlobal(structPtr);
                return obj;
            }
        }

        public class Formatter
        {
            public static String formatString(String originString, String charactersInString)
            {
                String formatString = "";

                foreach (char c in originString)
                {
                    if (charactersInString.Contains(c.ToString()))
                    {
                        formatString += c;
                    }
                }

                return formatString;
            }

            private static String dealWithString(String str)
            {
                String dealStr = "";

                int i = 0;
                foreach (char c in str)
                {

                    if (++i == 3)
                    {
                        dealStr += " ";
                        i = 1;
                    }
                    dealStr += c.ToString();

                }
                return dealStr;
            }

            public static String formatStringToHex(String originString, int limitLength, bool padding)
            {

                if (padding)
                {
                    originString = originString.Replace(" ", "");
                }

                String str = formatString(originString, "0123456789ABCDEFabcdef");

                if (str.Length > limitLength)
                {
                    str = str.Substring(0, limitLength);

                }

                if (str.Length == 0)
                {
                    str = "00";
                }

                if (padding)
                {
                    str = dealWithString(str);
                }

                return str;
            }

            public static void adjustTextFieldToHex(TextBox textBox, int limitLength, bool padding)
            {
                String originString = textBox.Text;
                if (padding)
                {
                    originString = originString.Replace(" ", "");
                }

                String str = formatString(originString, "0123456789ABCDEFabcdef");

                if (str.Length > limitLength)
                {
                    str = str.Substring(0, limitLength);

                }

                if (padding)
                {
                    str = dealWithString(str);
                }




                // update text
                textBox.Text = str;


            }
        }

        private void LangCombox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (LangCombox.SelectedIndex == 0 && Properties.Settings.Default.Language != "English")
            {
                currentLanguage = SystemLanguage.EnglishLanguage;
                Properties.Settings.Default.Language = "English";
                Properties.Settings.Default.Save();
                Thread.CurrentThread.CurrentUICulture = CultureInfo.GetCultureInfo("en-US");
                ApplyResources();

            }
            else if (LangCombox.SelectedIndex == 1 && Properties.Settings.Default.Language != "Chinese")
            {
                currentLanguage = SystemLanguage.ChineseLanguage;
                Properties.Settings.Default.Language = "Chinese";
                Properties.Settings.Default.Save();
                Thread.CurrentThread.CurrentUICulture = CultureInfo.GetCultureInfo("zh-Hans");
                ApplyResources();

            }

        }

        private void ApplyResources()
        {
            System.ComponentModel.ComponentResourceManager res = new System.ComponentModel.ComponentResourceManager(typeof(InnoMakerusb2Can));
            foreach (Control ctl in Controls)
            {
                res.ApplyResources(ctl, ctl.Name);
            }
            this.ResumeLayout(false);
            this.PerformLayout();
            res.ApplyResources(this, "$this");


            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {

                String[] Columns = { "序号", "时间戳", "渠道", "方向", "帧ID", "帧类型", "帧格式", "长度", "帧数据", "消息" };
                for (int i = 0; i < Columns.Length; i++)
                {
                    this.listView.Columns[i].Text = Columns[i];
                    if (i == Columns.Length - 1)
                    {
                        this.listView.Columns[i].Width = 200;
                    }
                    else
                    {
                        this.listView.Columns[i].Width = 120;
                    }
                }



                FrameIdTextBox.PlaceHolderText = "请输入4个16进制数字";

                DataTextBox.PlaceHolderText = "请输入8个16进制数字";

            }
            else
            {
                String[] Columns = { "SeqID", "TimeStamp", "Channel", "Direction", "FrameId", "FrameType", "FrameFormat", "Length", "FrameData", "Message" };
                for (int i = 0; i < Columns.Length; i++)
                {
                    this.listView.Columns[i].Text = Columns[i];
                    if (i == Columns.Length - 1)
                    {
                        this.listView.Columns[i].Width = 200;
                    }
                    else
                    {
                        this.listView.Columns[i].Width = 120;
                    }
                }



                FrameIdTextBox.PlaceHolderText = "Please Input Four Bytes Hex";

                DataTextBox.PlaceHolderText = "Please Input Eight Bytes Hex";
            }
        }

        private void FrameIdMouseEnter(object sender, EventArgs e)
        {
            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                this.toolTip1.Show("请输入4个16进制数字", FrameIdTextBox);

            }
            else
            {
                this.toolTip1.Show("Please input four hex number", FrameIdTextBox);
            }


        }

        private void FrameIdMouseLeave(object sender, EventArgs e)
        {
            this.toolTip1.Hide(FrameIdTextBox);
        }

        private void DateTextBoxMouseEnter(object sender, EventArgs e)
        {
            if (currentLanguage == SystemLanguage.ChineseLanguage)
            {
                this.toolTip1.Show("请输入8个16进制数字", DataTextBox);

            }
            else
            {
                this.toolTip1.Show("Please input eight hex number", DataTextBox);
            }
        }

        private void DateTextBoxMouseLeave(object sender, EventArgs e)
        {
            this.toolTip1.Hide(DataTextBox);
        }

        private void FormatComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            curFrameFormatSelectIndex = FormatComboBox.SelectedIndex;
        }

        private void TypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            curFrameTypeSelectIndex = TypeComboBox.SelectedIndex;
        }

        private void pictureBox1_Click(object sender, EventArgs e)
        {

        }
    }
}

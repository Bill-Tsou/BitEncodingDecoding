using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BitEncoderDecoderGUI
{
    static class Program
    {
        /// <summary>
        /// 應用程式的主要進入點。
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }

        public const bool SIM_ON_DECODE = false;

        public const string DecodeIdentifier = "Decoded Result: ";
        private const string SerialTerm = "\r";

        static private Form1 mainWindow = null;

        static public void RegisterForm1(Form1 new_form)
        { mainWindow = new_form; }

        static public void RefreshAvailablePorts()
        {
            mainWindow.comboBox_port.DataSource = SerialPort.GetPortNames();
        }

        static public bool ConnectComPort()
        {
            mainWindow.serialPort1.PortName = mainWindow.comboBox_port.Text;
            mainWindow.serialPort1.BaudRate = 115200;
            mainWindow.serialPort1.Parity = Parity.None;
            mainWindow.serialPort1.ReadTimeout = 20;
            mainWindow.serialPort1.WriteTimeout = 20;

            try {
                mainWindow.serialPort1.Open();
            } catch (Exception e) {
                mainWindow.textBox_port_msg.AppendText("\r\n[Connection Error]" + e.Message + "\r\n");
                return false;
            }
            return true;
        }

        static public bool DisconnectComPort()
        {
            try {
                mainWindow.serialPort1.Close();
            } catch (Exception e) {
                mainWindow.textBox_port_msg.AppendText("\r\n[Disconnect Error]" + e.Message + "\r\n");
                return false;
            }
            return true;
        }

        static public void SendSerialMessage(string msg)
        {
            if (mainWindow.serialPort1.IsOpen == false)
            {
                mainWindow.timer_decode_msg.Stop();
                MessageBox.Show(mainWindow, "Serial Port Disconnected!", mainWindow.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            // clear serial read and write buffer
            mainWindow.serialPort1.DiscardInBuffer();
            mainWindow.serialPort1.DiscardOutBuffer();
            mainWindow.textBox_port_msg.AppendText("[Send] >> " + msg + "\r\n");
            mainWindow.serialPort1.Write(msg + SerialTerm);
        }

        static public string ReadSerialResponse(bool extract_decode_msg = false)
        {
            if (mainWindow.serialPort1.IsOpen == false)
            {
                mainWindow.timer_decode_msg.Stop();
                return null;
            }
            string recv_msg = null;
            do
            {
                try
                {
                    recv_msg = mainWindow.serialPort1.ReadTo(SerialTerm);
                }
                catch (Exception ex) { }
                mainWindow.textBox_port_msg.AppendText("[Read] << " + ((recv_msg == null) ? "(No Response)" : recv_msg) + "\r\n");
                if (extract_decode_msg && (recv_msg != null) && recv_msg.Contains(DecodeIdentifier))
                    break;
            } while (extract_decode_msg && (recv_msg != null));
            
            return recv_msg;
        }

        static public string SendAndReadSerial(string msg)
        {
            SendSerialMessage(msg);
            return ReadSerialResponse();
        }
    }
}

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace BitEncoderDecoderGUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            // register Form1 object to Program once initialized
            Program.RegisterForm1(this);
        }

        private void Form1_Load(object sender, EventArgs e)
        {   // initialize available COM ports
            Program.RefreshAvailablePorts();

            //string str_test = "Decoded Result: 49 50 51 52 53 54 55 57";
            //str_test = str_test.Remove(0, "Decoded Result: ".Length);
        }

        private void button_connect_Click(object sender, EventArgs e)
        {
            if(serialPort1.IsOpen)
            {   // try to disconnect serial port
                if(Program.DisconnectComPort())
                {
                    textBox_port_msg.AppendText("Disconnect " + comboBox_port.Text + " successfully!" + "\r\n");
                    // change name of UI and states
                    comboBox_port.Enabled = true;
                    button_connect.Text = "&Connect";
                    button_refresh.Enabled = true;
                    // disable all items
                    radioButton_encode.Enabled = false;
                    textBox_encode_msg.Enabled = false;
                    button_encode_msg_send.Enabled = false;
                    radioButton_decode.Enabled = false;
                    trackBar_decode_th.Enabled = false;
                }
            }
            else
            {   // try to connect serial port
                textBox_port_msg.AppendText("Connecting to " + comboBox_port.Text + "...");
                if(comboBox_port.Text.Trim().Length == 0)
                {
                    textBox_port_msg.AppendText("No Available Devices!" + "\r\n");
                    MessageBox.Show(this, "No Available Devices! Please Check USB Connections", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else if(Program.ConnectComPort())
                {
                    textBox_port_msg.AppendText("OK" + "\r\n");
                    // change name of UI and states
                    comboBox_port.Enabled = false;
                    button_connect.Text = "Dis&connect";
                    button_refresh.Enabled = false;
                    radioButton_encode.Enabled = true;
                    radioButton_decode.Enabled = true;
                    // enable items
                    if (radioButton_encode.Checked)
                    {
                        textBox_encode_msg.Enabled = true;
                        button_encode_msg_send.Enabled = true;
                        radioButton_encode_CheckedChanged(sender, e);
                    }
                    else if(radioButton_decode.Checked)
                    {
                        trackBar_decode_th.Enabled = true;
                        label_decode_msg1.Text = "--------";
                        label_decode_msg2.Text = "--------";
                        radioButton_decode_CheckedChanged(sender, e);
                    }
                }
            }
        }

        private void button_refresh_Click(object sender, EventArgs e)
        { Program.RefreshAvailablePorts(); }

        private void checkBox_show_msg_CheckedChanged(object sender, EventArgs e)
        { textBox_port_msg.Visible = checkBox_show_msg.Checked; }

        private void radioButton_encode_CheckedChanged(object sender, EventArgs e)
        {
            if(radioButton_encode.Checked)
            {
                radioButton_encode.Checked = true;
                textBox_encode_msg.Enabled = true;
                button_encode_msg_send.Enabled = true;
                radioButton_decode.Checked = false;
                trackBar_decode_th.Enabled = false;
                // switch operation mode to encoding
                string response = Program.SendAndReadSerial("mode encode");
                if ((response != null) && (response != "OK"))
                    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);

                // stop decoding message inspection timer
                timer_decode_msg.Stop();
            }
        }

        private void radioButton_decode_CheckedChanged(object sender, EventArgs e)
        {
            if(radioButton_decode.Checked)
            {
                radioButton_encode.Checked = false;
                textBox_encode_msg.Enabled = false;
                button_encode_msg_send.Enabled = false;
                radioButton_decode.Checked = true;
                trackBar_decode_th.Enabled = true;
                label_decode_msg1.Text = "--------";
                label_decode_msg2.Text = "--------";
                // switch operation mode to decoding
                string response = Program.SendAndReadSerial("mode decode");
                if ((response != null) && (response != "OK"))
                    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                // set the threshold
                trackBar_decode_th_MouseCaptureChanged(sender, e);

                // start decoding message inspection timer
                timer_decode_msg.Start();
            }
        }

        private void button_encode_msg_send_Click(object sender, EventArgs e)
        {
            string encoding_msg = textBox_encode_msg.Text;
            if (encoding_msg.Length != 8)
            {
                MessageBox.Show(this, "Encoding Message Length must be 8!", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            string response = Program.SendAndReadSerial("encode " + encoding_msg);
            if((response != null) && (response.Contains("OK") == false))
                MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void textBox_encode_msg_KeyDown(object sender, KeyEventArgs e)
        {   // press Send button when Enter is pressed in textBox
            if(e.KeyCode == Keys.Enter)
                button_encode_msg_send_Click(sender, e);
        }

        private void trackBar_decode_th_Scroll(object sender, EventArgs e)
        {
            label_decode_th.Text = trackBar_decode_th.Value.ToString() + "%";
        }

        private void trackBar_decode_th_MouseCaptureChanged(object sender, EventArgs e)
        {   // send the message to decoder to set new selected threshold
            try {
                string response = Program.SendAndReadSerial("vi_th " + Convert.ToString(trackBar_decode_th.Value));
                if ((response != null) && (response != "OK"))
                    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            } catch (Exception exception) {
                MessageBox.Show(this, "Unable to Change Threshold, Error: " + exception.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void timer_decode_msg_Tick(object sender, EventArgs e)
        {   // read and display the decoded results details at once
            string response = Program.ReadSerialResponse(true);
            if ((response != null) && (response.Contains(Program.DecodeIdentifier)))
            {   // split the result and display on window
                response = response.Remove(0, Program.DecodeIdentifier.Length);
                string[] sep_resp = response.Split(' ');
                string res = "";
Console.WriteLine("length = " + sep_resp.Length.ToString());
                for (int i = 0; i < sep_resp.Length; i++)
                {
                    try
                    {
Console.WriteLine("idx = " + i.ToString() + ", data = " + sep_resp[i]);
                        res += ((Char)Convert.ToByte(sep_resp[i])).ToString();
                    }
                    catch (Exception ex)
                    { continue; }
                }
                if (decoded_text_index == 0)
                    label_decode_msg1.Text = res;
                else if (decoded_text_index == 1)
                    label_decode_msg2.Text = res;

                if ((++decoded_text_index) >= 2)
                    decoded_text_index = 0;
            }
        }

        // variable to toggle the current display text after decoded
        private int decoded_text_index = 0;
    }
}

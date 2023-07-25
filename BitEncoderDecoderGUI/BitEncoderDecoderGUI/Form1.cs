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
            // initialize array of data
            decoded_data_order_index = new int[Program.MAX_DECODED_DATA_NUMBER];
            for (int i = 0; i < decoded_data_order_index.Length; i++)
                decoded_data_order_index[i] = -1;
            decoded_data = new string[Program.MAX_DECODED_DATA_NUMBER];

            // register Form1 object to Program once initialized
            Program.RegisterForm1(this);
        }

        private void Form1_Load(object sender, EventArgs e)
        {   // initialize available COM ports
            Program.RefreshAvailablePorts();
            // initialize the comboBox selection
            comboBox_encode_id.SelectedIndex = 0;

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
                    comboBox_encode_id.Enabled = false;
                    textBox_encode_msg.Enabled = false;
                    button_encode_msg_send.Enabled = false;
                    radioButton_decode.Enabled = false;
                    trackBar_decode_trig_th.Enabled = false;
                    trackBar_decode_data_th.Enabled = false;
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
                        comboBox_encode_id.Enabled = true;
                        textBox_encode_msg.Enabled = true;
                        button_encode_msg_send.Enabled = true;
                        radioButton_encode_CheckedChanged(sender, e);
                    }
                    else if(radioButton_decode.Checked)
                    {
                        trackBar_decode_trig_th.Enabled = true;
                        trackBar_decode_data_th.Enabled = true;
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
                comboBox_encode_id.Enabled = true;
                textBox_encode_msg.Enabled = true;
                button_encode_msg_send.Enabled = true;
                radioButton_decode.Checked = false;
                trackBar_decode_trig_th.Enabled = false;
                trackBar_decode_data_th.Enabled = false;
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
                comboBox_encode_id.Enabled = false;
                textBox_encode_msg.Enabled = false;
                button_encode_msg_send.Enabled = false;
                radioButton_decode.Checked = true;
                trackBar_decode_trig_th.Enabled = true;
                trackBar_decode_data_th.Enabled = true;
                label_decode_msg1.Text = "--------";
                label_decode_msg2.Text = "--------";
                // switch operation mode to decoding
                string response = Program.SendAndReadSerial("mode decode");
                if ((response != null) && (response != "OK"))
                    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                // set the thresholds
                trackBar_decode_trig_th_MouseCaptureChanged(sender, e);
                trackBar_decode_th_MouseCaptureChanged(sender, e);

                // start decoding message inspection timer
                timer_decode_msg.Start();
            }
        }

        private void button_encode_msg_send_Click(object sender, EventArgs e)
        {
            // specify id number first
            string response = Program.SendAndReadSerial("id " + comboBox_encode_id.SelectedIndex.ToString());
            if((response != null) && (response.Contains("OK") == false))
            {
                MessageBox.Show(this, "Error while Setting Encoding ID Number!", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string encoding_msg = textBox_encode_msg.Text;
            if (encoding_msg.Length != 8)
            {
                MessageBox.Show(this, "Encoding Message Length must be 8!", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            response = Program.SendAndReadSerial("encode " + encoding_msg);
            if((response != null) && (response.Contains("OK") == false))
                MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void textBox_encode_msg_KeyDown(object sender, KeyEventArgs e)
        {   // press Send button when Enter is pressed in textBox
            if(e.KeyCode == Keys.Enter)
                button_encode_msg_send_Click(sender, e);
        }

        private void trackBar_decode_trig_th_Scroll(object sender, EventArgs e)
        {
            label_decode_trig_th.Text = trackBar_decode_trig_th.Value.ToString() + "%";
        }

        private void trackBar_decode_trig_th_MouseCaptureChanged(object sender, EventArgs e)
        {
            try
            {
                string response = Program.SendAndReadSerial("vi_th_trig " + Convert.ToString(trackBar_decode_trig_th.Value));
                // regardless the response message
                //if ((response != null) && (response != "OK"))
                //    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception exception)
            {
                MessageBox.Show(this, "Unable to Change Threshold, Error: " + exception.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void trackBar_decode_th_Scroll(object sender, EventArgs e)
        {
            label_decode_data_th.Text = trackBar_decode_data_th.Value.ToString() + "%";
        }

        private void trackBar_decode_th_MouseCaptureChanged(object sender, EventArgs e)
        {   // send the message to decoder to set new selected threshold
            try {
                string response = Program.SendAndReadSerial("vi_th_dec " + Convert.ToString(trackBar_decode_data_th.Value));
                // regardless the response message
                //if ((response != null) && (response != "OK"))
                //    MessageBox.Show(this, "Error: " + response, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            catch (Exception exception) {
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
                int index = 0;
                try
                {
                    // store ID of encoder
                    int decoded_index = Convert.ToInt32(sep_resp[index++]);
                    if (decoded_index < Program.MAX_DECODED_DATA_NUMBER)
                    {   // find not -1 value (empty) or duplicated index to put new decoded index
                        for(int i = 0; i < Program.MAX_DECODED_DATA_NUMBER; i++)
                            if((decoded_data_order_index[i] == -1) || (decoded_data_order_index[i] == decoded_index))
                            {
                                decoded_data_order_index[i] = decoded_index;
                                break;
                            }
                    }
                    else
                    {
                        MessageBox.Show(this, "[Error] Decoded Data ID " + decoded_index.ToString() + ", out of Range " +
                            Program.MAX_DECODED_DATA_NUMBER.ToString() + "!", this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
Console.WriteLine("ID = " + decoded_index.ToString());
                    // store the decoded data
                    for (; index < sep_resp.Length; index++)
                    {
                        try
                        {
                            Console.WriteLine("idx = " + index.ToString() + ", data = " + sep_resp[index]);
                            res += ((Char)Convert.ToByte(sep_resp[index])).ToString();
                        }
                        catch (Exception ex)
                        { continue; }
                    }

                    // store the decoded result
                    decoded_data[decoded_index] = res;

                    if(StripMenuItem_single_msg.Checked)
                    {   // display the single decoded result directly
                        label_decoded_id1.Text = "ID " + decoded_index.ToString();
                        label_decode_msg1.Text = res;
                    }
                    else
                    {   // scan if all decoded ID data has been received
                        bool all_decoded_data_arrived = true;
                        foreach (int data in decoded_data_order_index)
                            if (data == -1)
                            {
                                all_decoded_data_arrived = false;
                                break;
                            }
                        if (all_decoded_data_arrived)
                        {
                            // display the ID and corresponding results
                            label_decoded_id1.Text = "ID " + decoded_data_order_index[0].ToString();
                            label_decode_msg1.Text = decoded_data[decoded_data_order_index[0]];
                            label_decoded_id2.Text = "ID " + decoded_data_order_index[1].ToString();
                            label_decode_msg2.Text = decoded_data[decoded_data_order_index[1]];

                            // reset data arrival array
                            for (int i = 0; i < decoded_data_order_index.Length; i++)
                                decoded_data_order_index[i] = -1;
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(this, "[Error] Invalid Decoded Message: " + ex.Message, this.Text, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                
                /*if (decoded_text_index == 0)
                    if(Program.SIM_ON_DECODE)
                        label_decode_msg1.Text = "ABC-1234";
                    else
                        label_decode_msg1.Text = res;
                else if (decoded_text_index == 1)
                    if (Program.SIM_ON_DECODE)
                        label_decode_msg2.Text = "BCD-2345";
                    else
                        label_decode_msg2.Text = res;

                if ((++decoded_text_index) >= 2)
                    decoded_text_index = 0;
                */
            }
        }

        // variable to toggle the current display text after decoded
        private int[] decoded_data_order_index;
        private string[] decoded_data;

        private void StripMenuItem_single_msg_Click(object sender, EventArgs e)
        {
            StripMenuItem_single_msg.Checked = !StripMenuItem_single_msg.Checked;
            if (StripMenuItem_single_msg.Checked)
            {
                label_decoded_id2.Text = "----";
                label_decode_msg2.Text = "--------";
            }
            else
            {
                label_decoded_id2.Text = "ID -";
            }
        }

        private void Form1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                contextMenuStrip1.Show(Cursor.Position);
        }
    }
}


namespace BitEncoderDecoderGUI
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置受控資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 設計工具產生的程式碼

        /// <summary>
        /// 此為設計工具支援所需的方法 - 請勿使用程式碼編輯器修改
        /// 這個方法的內容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_refresh = new System.Windows.Forms.Button();
            this.checkBox_show_msg = new System.Windows.Forms.CheckBox();
            this.textBox_port_msg = new System.Windows.Forms.TextBox();
            this.button_connect = new System.Windows.Forms.Button();
            this.comboBox_port = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label_decode_msg = new System.Windows.Forms.Label();
            this.label_decode_th = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.trackBar_decode_th = new System.Windows.Forms.TrackBar();
            this.button_encode_msg_send = new System.Windows.Forms.Button();
            this.textBox_encode_msg = new System.Windows.Forms.TextBox();
            this.radioButton_decode = new System.Windows.Forms.RadioButton();
            this.radioButton_encode = new System.Windows.Forms.RadioButton();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.timer_decode_msg = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_decode_th)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.groupBox1.Controls.Add(this.button_refresh);
            this.groupBox1.Controls.Add(this.checkBox_show_msg);
            this.groupBox1.Controls.Add(this.textBox_port_msg);
            this.groupBox1.Controls.Add(this.button_connect);
            this.groupBox1.Controls.Add(this.comboBox_port);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(14, 14);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(5);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(5);
            this.groupBox1.Size = new System.Drawing.Size(465, 370);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "RS-232 Connection && Messages";
            // 
            // button_refresh
            // 
            this.button_refresh.Location = new System.Drawing.Point(332, 27);
            this.button_refresh.Name = "button_refresh";
            this.button_refresh.Size = new System.Drawing.Size(125, 31);
            this.button_refresh.TabIndex = 3;
            this.button_refresh.Text = "&Refresh";
            this.button_refresh.UseVisualStyleBackColor = true;
            this.button_refresh.Click += new System.EventHandler(this.button_refresh_Click);
            // 
            // checkBox_show_msg
            // 
            this.checkBox_show_msg.AutoSize = true;
            this.checkBox_show_msg.Location = new System.Drawing.Point(14, 67);
            this.checkBox_show_msg.Margin = new System.Windows.Forms.Padding(5);
            this.checkBox_show_msg.Name = "checkBox_show_msg";
            this.checkBox_show_msg.Size = new System.Drawing.Size(226, 24);
            this.checkBox_show_msg.TabIndex = 1;
            this.checkBox_show_msg.Text = "Show &Transmission Details";
            this.checkBox_show_msg.UseVisualStyleBackColor = true;
            this.checkBox_show_msg.CheckedChanged += new System.EventHandler(this.checkBox_show_msg_CheckedChanged);
            // 
            // textBox_port_msg
            // 
            this.textBox_port_msg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_port_msg.Location = new System.Drawing.Point(13, 101);
            this.textBox_port_msg.Margin = new System.Windows.Forms.Padding(5);
            this.textBox_port_msg.Multiline = true;
            this.textBox_port_msg.Name = "textBox_port_msg";
            this.textBox_port_msg.ReadOnly = true;
            this.textBox_port_msg.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox_port_msg.Size = new System.Drawing.Size(439, 259);
            this.textBox_port_msg.TabIndex = 1;
            this.textBox_port_msg.Visible = false;
            // 
            // button_connect
            // 
            this.button_connect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button_connect.Location = new System.Drawing.Point(199, 27);
            this.button_connect.Margin = new System.Windows.Forms.Padding(5);
            this.button_connect.Name = "button_connect";
            this.button_connect.Size = new System.Drawing.Size(125, 31);
            this.button_connect.TabIndex = 2;
            this.button_connect.Text = "&Connect";
            this.button_connect.UseVisualStyleBackColor = true;
            this.button_connect.Click += new System.EventHandler(this.button_connect_Click);
            // 
            // comboBox_port
            // 
            this.comboBox_port.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBox_port.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_port.FormattingEnabled = true;
            this.comboBox_port.Location = new System.Drawing.Point(65, 29);
            this.comboBox_port.Margin = new System.Windows.Forms.Padding(5);
            this.comboBox_port.Name = "comboBox_port";
            this.comboBox_port.Size = new System.Drawing.Size(124, 28);
            this.comboBox_port.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 32);
            this.label1.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 20);
            this.label1.TabIndex = 0;
            this.label1.Text = "Port:";
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.groupBox3);
            this.groupBox2.Controls.Add(this.label_decode_th);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.trackBar_decode_th);
            this.groupBox2.Controls.Add(this.button_encode_msg_send);
            this.groupBox2.Controls.Add(this.textBox_encode_msg);
            this.groupBox2.Controls.Add(this.radioButton_decode);
            this.groupBox2.Controls.Add(this.radioButton_encode);
            this.groupBox2.Location = new System.Drawing.Point(487, 14);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(497, 370);
            this.groupBox2.TabIndex = 1;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Operation Modes";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.label_decode_msg);
            this.groupBox3.Location = new System.Drawing.Point(56, 169);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(435, 191);
            this.groupBox3.TabIndex = 8;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Decoded Message";
            // 
            // label_decode_msg
            // 
            this.label_decode_msg.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label_decode_msg.Font = new System.Drawing.Font("Arial Rounded MT Bold", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_decode_msg.ForeColor = System.Drawing.Color.Blue;
            this.label_decode_msg.Location = new System.Drawing.Point(6, 25);
            this.label_decode_msg.Name = "label_decode_msg";
            this.label_decode_msg.Size = new System.Drawing.Size(423, 163);
            this.label_decode_msg.TabIndex = 7;
            this.label_decode_msg.Text = "--------";
            this.label_decode_msg.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label_decode_th
            // 
            this.label_decode_th.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label_decode_th.AutoSize = true;
            this.label_decode_th.Location = new System.Drawing.Point(450, 138);
            this.label_decode_th.Name = "label_decode_th";
            this.label_decode_th.Size = new System.Drawing.Size(41, 20);
            this.label_decode_th.TabIndex = 6;
            this.label_decode_th.Text = "40%";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(52, 137);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(176, 20);
            this.label2.TabIndex = 5;
            this.label2.Text = "V && I Clock Threshold:";
            // 
            // trackBar_decode_th
            // 
            this.trackBar_decode_th.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBar_decode_th.AutoSize = false;
            this.trackBar_decode_th.Enabled = false;
            this.trackBar_decode_th.Location = new System.Drawing.Point(234, 137);
            this.trackBar_decode_th.Maximum = 80;
            this.trackBar_decode_th.Minimum = 20;
            this.trackBar_decode_th.Name = "trackBar_decode_th";
            this.trackBar_decode_th.Size = new System.Drawing.Size(210, 35);
            this.trackBar_decode_th.TabIndex = 4;
            this.trackBar_decode_th.TickFrequency = 10;
            this.trackBar_decode_th.Value = 40;
            this.trackBar_decode_th.Scroll += new System.EventHandler(this.trackBar_decode_th_Scroll);
            this.trackBar_decode_th.MouseCaptureChanged += new System.EventHandler(this.trackBar_decode_th_MouseCaptureChanged);
            // 
            // button_encode_msg_send
            // 
            this.button_encode_msg_send.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button_encode_msg_send.Enabled = false;
            this.button_encode_msg_send.Location = new System.Drawing.Point(366, 60);
            this.button_encode_msg_send.Name = "button_encode_msg_send";
            this.button_encode_msg_send.Size = new System.Drawing.Size(125, 31);
            this.button_encode_msg_send.TabIndex = 3;
            this.button_encode_msg_send.Text = "&Send";
            this.button_encode_msg_send.UseVisualStyleBackColor = true;
            this.button_encode_msg_send.Click += new System.EventHandler(this.button_encode_msg_send_Click);
            // 
            // textBox_encode_msg
            // 
            this.textBox_encode_msg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox_encode_msg.Enabled = false;
            this.textBox_encode_msg.Location = new System.Drawing.Point(56, 62);
            this.textBox_encode_msg.Name = "textBox_encode_msg";
            this.textBox_encode_msg.Size = new System.Drawing.Size(304, 29);
            this.textBox_encode_msg.TabIndex = 2;
            this.textBox_encode_msg.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_encode_msg_KeyDown);
            // 
            // radioButton_decode
            // 
            this.radioButton_decode.AutoSize = true;
            this.radioButton_decode.Location = new System.Drawing.Point(21, 101);
            this.radioButton_decode.Name = "radioButton_decode";
            this.radioButton_decode.Size = new System.Drawing.Size(85, 24);
            this.radioButton_decode.TabIndex = 1;
            this.radioButton_decode.Text = "&Decode";
            this.radioButton_decode.UseVisualStyleBackColor = true;
            this.radioButton_decode.CheckedChanged += new System.EventHandler(this.radioButton_decode_CheckedChanged);
            // 
            // radioButton_encode
            // 
            this.radioButton_encode.AutoSize = true;
            this.radioButton_encode.Checked = true;
            this.radioButton_encode.Enabled = false;
            this.radioButton_encode.Location = new System.Drawing.Point(21, 27);
            this.radioButton_encode.Name = "radioButton_encode";
            this.radioButton_encode.Size = new System.Drawing.Size(83, 24);
            this.radioButton_encode.TabIndex = 0;
            this.radioButton_encode.TabStop = true;
            this.radioButton_encode.Text = "&Encode";
            this.radioButton_encode.UseVisualStyleBackColor = true;
            this.radioButton_encode.CheckedChanged += new System.EventHandler(this.radioButton_encode_CheckedChanged);
            // 
            // timer_decode_msg
            // 
            this.timer_decode_msg.Interval = 200;
            this.timer_decode_msg.Tick += new System.EventHandler(this.timer_decode_msg_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(996, 396);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Font = new System.Drawing.Font("微軟正黑體", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(136)));
            this.Margin = new System.Windows.Forms.Padding(5);
            this.MinimumSize = new System.Drawing.Size(870, 330);
            this.Name = "Form1";
            this.Text = "Bit Encoder Decoder GUI";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_decode_th)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.CheckBox checkBox_show_msg;
        private System.Windows.Forms.Button button_connect;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label_decode_msg;
        private System.Windows.Forms.Label label_decode_th;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TrackBar trackBar_decode_th;
        private System.Windows.Forms.Button button_encode_msg_send;
        private System.Windows.Forms.TextBox textBox_encode_msg;
        private System.Windows.Forms.RadioButton radioButton_decode;
        private System.Windows.Forms.RadioButton radioButton_encode;
        private System.Windows.Forms.Button button_refresh;
        public System.Windows.Forms.ComboBox comboBox_port;
        public System.IO.Ports.SerialPort serialPort1;
        public System.Windows.Forms.TextBox textBox_port_msg;
        public System.Windows.Forms.Timer timer_decode_msg;
    }
}


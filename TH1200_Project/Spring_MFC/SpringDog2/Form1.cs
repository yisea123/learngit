using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

namespace SpringDog2 {
    public partial class Form1 : Form {
        public Form1() {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e) {
            int num_random = get_num_from_textbox(textBox1);
            int num_machine = get_num_from_textbox(textBox2);
            int num_newDay = get_num_from_textbox(textBox3);

            int magic = ((num_machine % 1000000 + num_random) << 10) | num_newDay;

            int num_password = ((magic >> 20) << 20) |
                                (magic & 0x3ff) << 10 |
                                (magic >> 10) & 0x3ff;

            set_num_to_textbox(textBox4, num_password);

            Debug.WriteLine(num_random);
            Debug.WriteLine(num_machine);
            Debug.WriteLine(num_newDay);
        }

        private int get_num_from_textbox(TextBox t) {
            int num = 0;

            if(t.TextLength > 0)
                num = Int32.Parse(t.Text);

            return num;
        }

        private void set_num_to_textbox(TextBox t, int num) {
            t.Text = num.ToString();
        }
    }
}

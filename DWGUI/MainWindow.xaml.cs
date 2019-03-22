using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Win32;

namespace DWGUI {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window {

        private String programPath = @"DWCLI.exe ";
        private Uri cwd = new Uri(System.AppDomain.CurrentDomain.BaseDirectory);
        private String imageSourcePath = @"imageNow.png";
        private String imageDonePath = @"imageDone.png";
        private String qrPath = @"qr.png";

        private String imageAttackPath = @"imageAttack.png";
        private String qrAttackPath = @"qrAttack.png";

        private About about = new About();
        private Attack attack = new Attack();

        public MainWindow() {
            InitializeComponent();
        }

        private void Click_About(object sender, RoutedEventArgs e) {
            about = new About();
            about.Show();
        }

        private void Click_Attack_HighPassFilter(object sender, RoutedEventArgs e) {
            AttackSimulate("highPassFilter");
        }

        private void Click_Attack_Noise(object sender, RoutedEventArgs e) {
            AttackSimulate("noise");
        }

        private void Click_Attack_Cut(object sender, RoutedEventArgs e) {
            AttackSimulate("cutting");
        }

        private void AttackSimulate(String attackName) {
            Log("Attack testing...");
            Run($"test -a {imageDonePath} -b {imageAttackPath} --{attackName}");
            Run($@"extract -f {imageDonePath} -o {qrPath} -n b -c 10");
            Run($@"extract -f {imageAttackPath} -o {qrAttackPath} -n b -c 20");
            attack = new Attack();
            attack.read(imageDonePath, imageAttackPath, qrPath, qrAttackPath);
            Log("Attack Done.");
            attack.Show();
        }

        private void Button_Click_Open(object sender, RoutedEventArgs e) {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            var result = openFileDialog.ShowDialog();
            string fileName = openFileDialog.FileName;
            imageSourcePath = openFileDialog.FileName;
            SourceImage.Source = GetBitmapImage(fileName);
            Log($@"Load from {fileName}");
        }

        private void Button_Click_Save(object sender, RoutedEventArgs e) {
            var saveFileDialog = new Microsoft.Win32.SaveFileDialog(); 
            saveFileDialog.Filter = "PNG(*.png)|*.png|JPG(*.jpg)|*.jpg"; 
            var result = saveFileDialog.ShowDialog();
            if (result == true) {
                var fileName = saveFileDialog.FileName;
                
                System.IO.File.Copy(
                    imageDonePath, fileName, true);
                Log($@"Save to {fileName}");

            }

        }

        private void Button_Click_Reset(object sender, RoutedEventArgs e) {

        }

        private void Button_Click_Add(object sender, RoutedEventArgs e) {
            SourceImage.Source = null;
            Log($@"Signing...");
            Run($@"qr -t {TextBox.Text} -o {qrPath}");
            Run($@"sign -c b -p {PowerText.Text} -m {MatchingText.Text} -o {imageSourcePath} -w {qrPath} -r {imageDonePath}");
            String values = Run($"test -a {imageSourcePath} -b {imageDonePath} -c");
            Log(values);
            SourceImage.Source = GetBitmapImage(imageDonePath);
            TextBox.Text = "";
            Log($@"Signed Success.");
        }

        private void Button_Click_Pause(object sender, RoutedEventArgs e) {

        }

        private void Button_Click_Start(object sender, RoutedEventArgs e) {

        }

        private void Button_Click_Extract(object sender, RoutedEventArgs e) {
            Log($@"Extracting...");
            bool debug = IsDebug();
            String debugOption = debug ? "-d" : "";
            Run($@"extract {debugOption} -f {imageSourcePath} -o {qrPath} -n b -c 10");
            String text = Run($@"qr -s -o {qrPath}");
            TextBox.Text = text;

            Log($@"Extracted Success.");
        }

        private String Run(String cmd) {
            Execute(programPath + cmd + " > output.txt");
            StreamReader sr = new StreamReader("output.txt");
            String text = sr.ReadToEnd();
            sr.Close();
            return text;
        }

        private String Execute(String cmd) {
            Process p = new Process();
            p.StartInfo.FileName = "cmd.exe";
            p.StartInfo.UseShellExecute = false;
            p.StartInfo.RedirectStandardInput = true;
            p.StartInfo.RedirectStandardOutput = true;
            p.StartInfo.RedirectStandardError = true;
            p.StartInfo.CreateNoWindow = true;
            p.Start();
            
            if (IsDebug()) Log(cmd);
            p.StandardInput.WriteLine(cmd);
            
            p.StandardInput.Close();
            
            string strOuput = p.StandardOutput.ReadToEnd();
            p.WaitForExit();
            p.Close();

            return strOuput;
        }

        private void Log(String message) {
            Console.WriteLine(message);
            LogBox.Text += $"{message}\n";
            ScrollViewer.ScrollToEnd();
        }

        private bool IsDebug() {
            return DebugCheckBox.IsChecked != null && (bool) DebugCheckBox.IsChecked;
        }

        private BitmapImage GetBitmapImage(string path) {
            BitmapImage bitmap = new BitmapImage();
            bitmap.BeginInit();
            bitmap.CacheOption = BitmapCacheOption.OnLoad;
            bitmap.StreamSource = new MemoryStream(File.ReadAllBytes(path));
            bitmap.EndInit();
            bitmap.Freeze();
            return bitmap;
        }

       
    }
}

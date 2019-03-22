using System;
using System.Collections.Generic;
using System.ComponentModel;
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
using System.Windows.Shapes;

namespace DWGUI {
    /// <summary>
    /// Interaction logic for Attack.xaml
    /// </summary>
    public partial class Attack : Window {
        private Uri cwd = new Uri(System.AppDomain.CurrentDomain.BaseDirectory);

        public Attack() {
            InitializeComponent();

        }

        public void read(String imageA, String imageB, String qrA, String qrB) {
            
            ImageA.Source = GetBitmapImage(imageA);
            ImageB.Source = GetBitmapImage(imageB);
            QrA.Source = GetBitmapImage(qrA);
            QrB.Source = GetBitmapImage(qrB);
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

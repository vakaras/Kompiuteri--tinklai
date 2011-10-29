#include "mime.h"


using namespace std;
using namespace smtp::mime;
using namespace smtp::encoders;
using namespace smtp::utils;


void MIMEBase::add_header(string type, string value) {
  this->headers[type] = value;
  }


void MIMEBase::add_utf8_header(string type, string value) {
  this->headers[type] = word_encode(value);
  }

void MIMEBase::write_headers(FILE *fp) {
  for (map<string, string>::iterator i = this->headers.begin();
      i != this->headers.end(); i++) {
    fprintf(fp, "%s: %s\n", i->first.c_str(), i->second.c_str());
    }
  }

MIMEHTML::MIMEHTML(string filename) {
  this->filename = filename;
  this->add_header("Content-Type", "text/html; charset=utf-8");
  this->add_header("Content-Transfer-Encoding", "base64");
  }

void MIMEHTML::write(FILE *fp) {
  MIMEBase::write_headers(fp);
  string data = read_file(this->filename);
  fprintf(fp, "\n%s\n", base64_encode(data).c_str());
  }

MIMEImage::MIMEImage(string filename, string content_id) {
  this->filename = filename;
  this->add_header("Content-Type", "image/jpeg");
  this->add_header("Content-Transfer-Encoding", "base64");
  this->add_header("Content-ID", "<" + content_id + ">");
  }

void MIMEImage::write(FILE *fp) {
  MIMEBase::write_headers(fp);
  string data = read_file(this->filename);
  fprintf(fp, "\n%s\n", base64_encode(data).c_str());
  }

MIMEMultipart::MIMEMultipart(string subject, string recipient) {
  this->add_header("To", recipient);
  this->add_utf8_header("Subject", subject);
  this->add_header("MIME-Version", "1.0 (mime-construct 1.9)");
  this->add_header(
      "Content-Type", "multipart/mixed; boundary=\"SKIRTUKAS\"");
  this->size = 0;
  }

void MIMEMultipart::write(FILE *fp) {
  MIMEBase::write_headers(fp);

  fprintf(fp, "This is a message with multiple parts in MIME format.\n");

  for (int i = 0; i < this->size; i++) {
    fprintf(fp, "\n--SKIRTUKAS\n");
    this->parts[i]->write(fp);
    }

  fprintf(fp, "\n--SKIRTUKAS--\n");
  }

void MIMEMultipart::append(MIMEBase *part) {
  parts[size++] = part;
  }

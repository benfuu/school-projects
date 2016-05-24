package com.blog;

import com.googlecode.objectify.ObjectifyService;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.*;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.AddressException;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;

/**
 * Created by Ben on 2/15/2016.
 */
public class DailyDigestServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        log("Executing CRON request\n");

        /* Get posts within 24 hours */
        Calendar day = Calendar.getInstance();
        day.add(Calendar.DATE, -1);
        Date date = day.getTime();
        List<Post> posts = ObjectifyService.ofy().load().type(Post.class).filter("date >", date).order("-date").list();
        List<Subscriber> subscribers = ObjectifyService.ofy().load().type(Subscriber.class).list();
        if (subscribers.size() == 0) { return; }    // don't send email if nobody is subscribed
        if (posts.size() > 0) { // send email
            /* Email Stuff */
            Properties props = new Properties();
            Session session = Session.getDefaultInstance(props, null);

            DateFormat df = new SimpleDateFormat("MMMM dd, yyyy 'at' h:mm a");
            df.setTimeZone(TimeZone.getTimeZone("America/Chicago"));
            StringBuilder msgBody = new StringBuilder();
            msgBody.append("<h4>Here are new posts from the last 24 hours. To view all posts, click <a href='http://ee461l-blog-byf69.appspot.com/'>here</a>.</h4>");
            for (Post post : posts) {
                msgBody.append("<h4>");
                msgBody.append(post.title);
                msgBody.append("</h4>");
                msgBody.append("<p>Written by ");
                msgBody.append(post.author);
                msgBody.append(" on ");
                msgBody.append(df.format(post.date));
                msgBody.append("</p><p>");
                msgBody.append(post.content.getValue());
                msgBody.append("</p>");
                msgBody.append("<br />");
            }

            try {
                Message msg = new MimeMessage(session);
                msg.setFrom(new InternetAddress("admin@ee461l-blog-byf69.appspotmail.com", "Admin - EE 461L Blog"));
                log(subscribers.toString());
                for (Subscriber subscriber : subscribers) {
                    msg.addRecipient(Message.RecipientType.TO, new InternetAddress(subscriber.email));
                }
                msg.setSubject("EE 461L Blog Digest");
                msg.setContent(
                        msgBody.toString(),
                        "text/html");
                Transport.send(msg);

            } catch (AddressException e) {
                // ...
                e.printStackTrace();
                throw new RuntimeException(e);
            } catch (MessagingException e) {
                // ...
                e.printStackTrace();
                throw new RuntimeException(e);
            }
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        doPost(request, response);
    }
}

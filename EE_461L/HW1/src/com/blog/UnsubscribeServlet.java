package com.blog;

import com.googlecode.objectify.Key;
import com.googlecode.objectify.ObjectifyService;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

/**
 * Created by Ben on 2/19/2016.
 */
public class UnsubscribeServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        log("in unsubscribe servlet");
        String action = request.getParameter("action");
        if (action.equals("Unsubscribe")) {
            log(request.getParameter("email"));
            List<Subscriber> subscribers = ObjectifyService.ofy()
                    .load()
                    .type(Subscriber.class)
                    .filter("email =", request.getParameter("email"))
                    .list();
            /*
            List<Key<Subscriber>> keys = ObjectifyService.ofy().load().type(Subscriber.class).keys().list();
            ObjectifyService.ofy().delete().keys(keys).now();
            */
            log(subscribers.toString());
            for (Subscriber sub : subscribers) {
                log(sub.email);
            }
            ObjectifyService.ofy().delete().entities(subscribers).now();  // synchronous
            /*
            Subscriber subscriber = ObjectifyService.ofy().load().type(Subscriber.class).filter("email ==", request.getParameter("email")).first().now();
            log("found match: " + subscriber.email);
            ObjectifyService.ofy().delete().entity(subscriber).now();
            */
            request.getRequestDispatcher("index.jsp").forward(request, response);
        }
        else {
            request.getRequestDispatcher("index.jsp").forward(request, response);
        }
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        doPost(request, response);
    }
}

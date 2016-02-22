package com.blog;

import com.googlecode.objectify.ObjectifyService;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.List;
import java.util.Properties;

/**
 * Created by Ben on 2/19/2016.
 */
public class SubscribeServlet extends HttpServlet {
    protected void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        String email = request.getParameter("email");
        List<Subscriber> matches = ObjectifyService.ofy().load().type(Subscriber.class).filter("email =", email).list();
        if (matches.size() == 0) {  // if matched, then don't add
            Subscriber subscriber = new Subscriber(email);
            ObjectifyService.ofy().save().entity(subscriber).now();
            log("successfully saved email: " + subscriber.email);
        }
        request.setAttribute("subscribed", "true");
        request.getRequestDispatcher("index.jsp").forward(request, response);
    }

    protected void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
        doPost(request, response);
    }
}
